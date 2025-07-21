#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <wait.h>
#include <pthread.h>
#include <sys/epoll.h>

#define IP      "0.0.0.0"
#define PORT    12345
#define BACKLOG 128
#define INET_ADDRLENGTH 16
#define BUF_SIZE 1024
#define MAX_EVENTS 1024

/*描述就绪文件描述符相关信息*/
struct myevent_s {
    int fd;                                 
    int events;
    void *arg;
    void (*call_back)(int fd, int events, void *arg);//arg存自定义结构体本身
    int status;
    char buf[BUFSIZ];
    int len;
    long last_active;
};
void cb_recv_data(int fd, int events, void *arg);
void event_add(int epfd, int events, struct myevent_s *expect_event);
int g_efd;   //全局变量保存epoll_creare返回的文件描述符
struct myevent_s g_events[MAX_EVENTS + 1]; //自定义结构体数组. +1是为了多存一个lfd


//用于给事件自定义结构体赋值
void event_set(struct myevent_s *expect_event, 
                int fd, int events, void *arg, 
                void (*call_back)(int, int, void *)) {
    expect_event -> fd = fd;
    expect_event -> events = events;
    expect_event -> arg = arg;
    expect_event -> call_back = call_back;
    expect_event -> status = 0;
    memset(expect_event -> buf, '\0', BUFSIZ);
    expect_event -> len = 0;
    expect_event -> last_active = time(NULL);

}

void event_del(int epfd, struct myevent_s *expect_event) {
    struct epoll_event tmp;
    if (expect_event -> status != 1) {
        return ;
    }
    tmp.data.ptr = NULL;
    expect_event -> status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL,  expect_event -> fd, &tmp);
    

}

void cb_send_data(int fd, int events, void *arg) {
    struct myevent_s *expect_event = arg;
    int ret = send(fd, expect_event -> buf, expect_event -> len, 0);
    event_del(g_efd, expect_event);
    if (ret > 0) {
        printf("send fd=%d: %s\n ", fd, expect_event -> buf );
        event_set(expect_event, fd, EPOLLIN, expect_event, cb_recv_data);   
        event_add(g_efd, EPOLLIN, expect_event); 
    }

}

void cb_recv_data(int fd, int events, void *arg) {
    
    struct myevent_s *expect_event = (struct myevent_s *)arg;
    event_del(fd, expect_event);
    int ret = recv(fd, expect_event -> buf, BUFSIZ, 0);
    if (ret > 0) {
        expect_event -> len = ret;
        expect_event -> buf[ret] = '\0';
        printf("%s\n", expect_event -> buf);
        event_set(expect_event, fd, EPOLLOUT, expect_event, cb_send_data);
        event_add(g_efd, EPOLLOUT, expect_event);
    } else if (ret == 0) {
        close(fd);
        printf("fd = %d,对端关闭\n", fd);
    } else {
        perror("recv error");
    }
}
//用于监听socket的回调，监听socket每成功连接一个客户端就调用这个函数
void cb_accept_conn(int fd, int events, void *arg) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int cfd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (cfd == -1) {
        perror("accept error");
        return;
    }
    int client_port;
    char client_ip[INET_ADDRLENGTH];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRLENGTH);
    client_port = ntohs(client_addr.sin_port);

   
    int i;
    //将新生成的cfd绑定到自定义事件结构体
    for (i = 0; i < MAX_EVENTS; i++) {
        //寻找一个最小且不在监听红黑树上的下标
        if (g_events[i].status == 0) {
            break; //如果在i时status的值为0，说明下标i未使用
        }
        
    }

    //此时i为上面for循环break或循环达到最大 i=MAX_EVENTS 的大小
    if (i == MAX_EVENTS) {
        fprintf(stderr, "too many connection:accept error");
        return;
    }

    //新生成的cfd也设置非堵塞
    int ret = 0;
    if ((ret = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
        perror("fcntl errror :cfd");
        return;
    }

    //将新生成的cfd也绑定到自定义事件结构体中
    event_set(&g_events[i], cfd, EPOLLIN, &g_events[i], cb_recv_data );
    //将新生成的cfd绑定到自定义事件结构体后，将该结构体放到监听红黑树上监听；
    event_add(g_efd, EPOLLIN, &g_events[i]);
     printf("%s:%d已经成功连接\n", client_ip, client_port);
}

//用于将事件自定义结构体添加到 epoll_create()来监听; 
void event_add(int epfd, int events, struct myevent_s *expect_event) {

    struct epoll_event ep_node;

    ep_node.data.ptr = expect_event;
    ep_node.events = expect_event -> events;

    if (expect_event -> status == 0) {
        expect_event -> status = 1;
    }
    if ((epoll_ctl(g_efd, EPOLL_CTL_ADD, expect_event -> fd, &ep_node)) < 0) {
        fprintf(stderr, "event add error [fd = %d] ,event[%0x]\n", expect_event -> fd, events);
    } else {
             fprintf(stdout, "event add success [fd = %d] ,event[%0x]\n", expect_event -> fd, events);
    }
    return;
}


//初始化服务器
void init_listen_socket() {

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    int ret = fcntl(lfd, F_SETFL, O_NONBLOCK); //设置监听为非阻塞；
    if (ret == -1) {
        perror("bind");
        exit(1);
    }
    
    /*---给服务器地址结构体赋值---*/
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);


    if ((bind(lfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1) {
        perror("bind");
        exit(1);
    }
    listen(lfd, BACKLOG);
    
    //将lfd绑定到自定义结构体中，用struct myevent_s g_events[MAX_EVENTS + 1]数组的最后一个元素
    event_set(&g_events[MAX_EVENTS], lfd, EPOLLIN, &g_events[MAX_EVENTS], cb_accept_conn);
    

}




int main(int argc, char  *argv[])
{
    g_efd = epoll_create(MAX_EVENTS + 1);
    struct epoll_event events[MAX_EVENTS];
    //服务器开始运行，先将lfd放到g_efd的红黑树上监听
    init_listen_socket();


    while (1) {
        int i = 0;
        //开始监听,nfd为就绪事件的个数，后面用作循环变量
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0) {

            perror("epoll_wait error");
            break;
        }

        //循环处理已就绪事件，nfd为epoll_wait的函数返回值
        for (i = 0; i < nfd; ++i) {

            struct myevent_s *expect_event = (struct myevent_s *)events[i].data.ptr;

            if (events[i].events & EPOLLIN && expect_event -> events & EPOLLIN) {

                //第一个参数，为要处理的就绪事件的文件描述符
                //第二个参数，要处理就绪时间的类型 读还是写？
                //第三个参数，要传给回调函数的参数arg，都在自定义结构体中

                expect_event -> call_back(expect_event -> fd, expect_event -> events, expect_event -> arg);
            }      

            if (events[i].events & EPOLLOUT && expect_event -> events & EPOLLOUT) {
           
                expect_event -> call_back(expect_event -> fd, expect_event -> events, expect_event -> arg);
            }  

        } //for
        

    }//while(1)
    

    return 0;
}