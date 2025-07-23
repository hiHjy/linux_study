#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>

#define IP      "0.0.0.0"
#define PORT    12345
#define BACKLOG 128
#define BUFSIZE 1024
#define MAX_EVENTS 1024

struct myevent_s {
    int fd;
    uint32_t  events;
    void *arg;
    void (*call_back)(int fd, int events, void *arg);
    int status;
    char buf[BUFSIZE];
    int len;
    long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENTS + 1];

// 前置声明
void event_set(struct myevent_s *ev, int fd, int events, void *arg, 
               void (*call_back)(int, int, void *));
void event_add(int epfd, int events, struct myevent_s *ev);
void event_del(int epfd, struct myevent_s *ev);
void cb_accept_conn(int fd, int events, void *arg);
void cb_recv_data(int fd, int events, void *arg);
void cb_send_data(int fd, int events, void *arg);

void event_set(struct myevent_s *ev, int fd, int events, void *arg,
               void (*call_back)(int, int, void *)) {
    ev->fd = fd;
    ev->events = events;
    ev->arg = arg;
    ev->call_back = call_back;
    if (events & EPOLLIN) {
        memset(ev->buf, '\0', BUFSIZE);
        ev->len = 0;
    }
  
    ev->status = 0;
    
    ev->last_active = time(NULL);
}



void cb_send_data(int fd, int events, void *arg) {
   
    struct myevent_s *tmp = arg;
    
    //
    if ( epoll_ctl(g_efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
        perror("epoll_ctl:EPOLL_CTL_DEL ");
        return;
    }

    tmp->status = 0;
    printf("buf=%s, len = %d\n", tmp -> buf, tmp->len);   
    int len = send(fd, tmp->buf, tmp->len, 0 );
    if (len == -1 && errno == EAGAIN) {
        perror("不可写");
           
    } else if (len > 0) {
        printf("写回客户端%d字节\n", len);
          
    } else {
        perror("send error");
         
    }   

    
    printf("cfd=%d的客户端被移出了监听树\n", fd);
    event_set(tmp, fd, EPOLLIN | EPOLLET, tmp, cb_recv_data);
            //设置好了那就再次加到树上吧
    event_add(g_efd, EPOLLIN | EPOLLET, tmp);



}

void cb_recv_data(int fd, int events, void *arg) {
    struct myevent_s *tmp = arg;
    //写过一次后将监听节点移出监听树吧，改为监听写吧,得先移除麻对吧？
        
    if ( epoll_ctl(g_efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
            perror("epoll_ctl:EPOLL_CTL_DEL ");
            return;
    }
    tmp->status = 0;
    printf("cfd=%d的客户端被移出了监听树\n", fd);
    //接受客户端的数据
    while (1) {
        int len = recv(fd, tmp->buf, BUFSIZE, 0);
        if (len > 0) {
            tmp->len = len;
            //如果len>0,恭喜我成功收到了客户端发来的数据
            write(STDOUT_FILENO, tmp->buf, len);
            printf("%s\n", tmp->buf);
            //写过一次后让服务器给cfd写吧设置监听cfd写，并设置回调cb_send_data
            event_set(tmp, fd, EPOLLOUT | EPOLLET, tmp, cb_send_data);
            //设置好了那就再次加到树上吧
            event_add(g_efd, EPOLLOUT | EPOLLET, tmp);

            
        } else if (len == 0) {
            printf("cfd=%d的客户端关闭了连接\n ", fd);
            break;
        } else if (errno == EAGAIN) {
            break;
        } else {
            perror("recv error");
            break;
        }

    } //while(1)
    
}


void event_add(int epfd, int events, struct myevent_s *ev) {
   struct epoll_event tmp; //要传给内核一个结构化的东西，所以用一个struct epoll_event类型的结构体
   tmp.data.ptr = ev;
   tmp.events = events; //让内核知道你要监听的是这个文件描述符的读还是写
   
   //先判断要监听的文件描述符，是不是已经在树上了
   if (ev->status == 1) {
        //该文件描述符已经被挂到树上了，修改就行了
        int ret = epoll_ctl(g_efd, EPOLL_CTL_MOD, ev->fd, &tmp) == 0;
        assert(ret != -1);

   } else {
        //该文件描述符第一次监听，好耶！赶紧挂到树上
        int ret = epoll_ctl(g_efd, EPOLL_CTL_ADD, ev->fd, &tmp) == 0;
        ev->status = 1;
        assert(ret != -1);
   }
   char *str;
   if (ev->events & (EPOLLIN )) { str = "EPOLLIN | EPOLLET";}
   else if (ev->events & (EPOLLOUT )) { str = "EPOLLOUT | EPOLLET";}
   printf("cfd = %d已经成功添加到监听树上, 监听时间类型:%s\n", ev->fd, str);
  
}

void cb_accept_conn(int fd, int events, void *arg) {
   //有新的连接，要创建新的socket才行哦
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int cfd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
    assert(cfd != -1);
    
    int flag = fcntl(cfd, F_GETFL);
    if (fcntl(cfd, F_SETFL, flag | O_NONBLOCK)) {
        perror("fcntl cfd");
        close(cfd);
        return;
    }
    
    //新的连接创建完毕了，用cfd与客户端通信，客户端的地址结构体也有啦----client_addr
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    printf("%s:%d 连接成功! cfd = %d\n ", ip, ntohs(client_addr.sin_port), cfd);

    //新的与客户端通信的socket已经创建成功啦，下一步要把它添加到监听树才行
        //1.从全局的自定义数组中找一个空位
        int i;
        for (i = 0; i < MAX_EVENTS; i++) { //用MAX_EVENTS的原因是g_events【MAX_EVENTS】已经被lfd占用了
            if (g_events[i].status == 0) break;
        }

        if (i == MAX_EVENTS) {
            perror("too many connections");
            close(fd);
            return;
        }
        //2.找到空位后，绑定cfd到自定义结构体上,要设置回调函数了，刚开始建立连接，一开始坑定是要先接受数据,所以绑定读回调
        event_set(&g_events[i], cfd, events, &g_events[i], cb_recv_data);
        //3.设置好回调后，将它挂到监听树上
        event_add(g_efd, events, &g_events[i]);

}

int main() {
    
    g_efd = epoll_create1(0);
    if (g_efd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    
    // 创建监听socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // 设置SO_REUSEADDR
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 绑定地址
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 监听
    if (listen(lfd, BACKLOG) == -1) {
        perror("listen");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 设置非阻塞
    int flags = fcntl(lfd, F_GETFL);
    if (flags == -1 || fcntl(lfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    /*--------------------------将监听socket添加到监听树上，并设置回调函数为cb_accept_conn()-------------------------*/
    // event_set(struct myevent_s *ev, int fd, int events, void *arg,
    //            void (*call_back)(int, int, void *)) 
    event_set(&g_events[MAX_EVENTS], lfd, EPOLLIN | EPOLLET,&g_events[MAX_EVENTS], cb_accept_conn);
    event_add(g_efd, EPOLLIN | EPOLLET, &g_events[MAX_EVENTS]);

   /*---------------------------------------------------------------------------------------------------*/ 
    int checkpos = 0, i;
    //连接五秒的客户端后还不发送数据，那么就踢掉你
    while (1) {
        for (i = 0; i < 100; i++) {
            int idx = checkpos;
            checkpos = (checkpos + 1) % MAX_EVENTS;
            if (g_events[idx].status == 0) {
                continue;
            }
            int duration =  time(NULL) - g_events[idx].last_active;
            
            if (duration > 5) {
                
                send(g_events[idx].fd, "连接超时,连接时长\n", strlen("连接超时\n"), 0);
                g_events[idx].status = 0;
                epoll_ctl(g_efd, g_events[idx].fd, EPOLL_CTL_DEL, NULL);
                close(g_events[idx].fd);
                printf("cfd=%d的客户端,已连接时长%d,已经被踢出,\n", g_events[idx].fd, duration);
                
            } 

        }

        //readys数组给epoll_wait用，用于传出处于监听树上的节点有哪些节点响应，nfds为响应节点的数量，后面用于处理响应
        struct epoll_event readys[MAX_EVENTS + 1];
        int nfds = epoll_wait(g_efd, readys, MAX_EVENTS + 1, 1000);
        
        
        //处理响应
        int i;
        for (i = 0; i < nfds; i++) {
            struct myevent_s *tmp = readys[i].data.ptr; //用于获取节点的自定义结构体
            if ((readys[i].events & (EPOLLIN | EPOLLET)) /*节点实际响应的事件*/ && (tmp->events & (EPOLLIN | EPOLLET))/*用户想要响应的事件*/ ) {
                tmp->call_back(tmp->fd, tmp->events, tmp->arg);
            } 

            if ((readys[i].events & (EPOLLOUT | EPOLLET)) /*节点实际响应的事件*/ && (tmp->events & (EPOLLOUT | EPOLLET))/*用户想要响应的事件*/ ) {
                tmp->call_back(tmp->fd, tmp->events, tmp->arg);
            } 


        }
        
        
       
    }//while(1)

    
    close(g_efd);
    return 0;
}