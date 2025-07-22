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

#define IP      "0.0.0.0"
#define PORT    12345
#define BACKLOG 128
#define BUFSIZE 1024
#define MAX_EVENTS 1024

struct myevent_s {
    int fd;
    int events;
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
    ev->status = 0;
    ev->len = 0;
    ev->last_active = time(NULL);
}

void event_del(int epfd, struct myevent_s *ev) {
    if (ev->status == 1) {
        if (epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL) == -1) {
            perror("epoll_ctl del error");
        } else {
            ev->status = 0;
            printf("Event removed: fd=%d\n", ev->fd);
        }
    }
}

void cb_send_data(int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    
    // 确保只处理写事件
    if (!(events & EPOLLOUT)) return;
    
    int total_sent = 0;
    while (total_sent < ev->len) {
        int ret = send(fd, ev->buf + total_sent, ev->len - total_sent, 0);
        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 部分发送，保存进度等待下次可写
                ev->len -= total_sent;
                memmove(ev->buf, ev->buf + total_sent, ev->len);
                event_add(g_efd, EPOLLOUT, ev);
                printf("Partial send: %d bytes sent, %d bytes remaining\n", total_sent, ev->len);
                return;
            } else {
                perror("send error");
                break;
            }
        }
        total_sent += ret;
    }
    
    if (total_sent > 0) {
        printf("Send success to fd=%d: %.*s\n", fd, total_sent, ev->buf);
    }
    
    // 重置为读事件
    memset(ev->buf, 0, BUFSIZE);
    ev->len = 0;
    event_set(ev, fd, EPOLLIN, ev, cb_recv_data);
    event_add(g_efd, EPOLLIN, ev);
}

void cb_recv_data(int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    
    // 确保只处理读事件
    if (!(events & EPOLLIN)) return;
    
    int ret = recv(fd, ev->buf + ev->len, BUFSIZE - ev->len, 0);
    if (ret > 0) {
        ev->len += ret;
        // 检查是否收到完整消息（这里以换行符为结束标志）
        char *end = memchr(ev->buf, '\n', ev->len);
        if (end) {
            // 收到完整消息，准备发送响应
            *end = '\0'; // 替换换行符为字符串结束符
            printf("Received complete message from fd=%d: %s\n", fd, ev->buf);
            
            // 准备响应（原样返回）
            int resp_len = end - ev->buf;
            memcpy(ev->buf, "Response: ", 10);
            memcpy(ev->buf + 10, ev->buf, resp_len);
            ev->len = resp_len + 10;
            ev->buf[ev->len] = '\n'; // 添加换行符作为结束
            
            // 切换到写事件
            event_set(ev, fd, EPOLLOUT, ev, cb_send_data);
            event_add(g_efd, EPOLLOUT, ev);
        } else {
            // 消息不完整，继续等待数据
            printf("Partial message from fd=%d: %d bytes\n", fd, ev->len);
            event_add(g_efd, EPOLLIN, ev);
        }
    } else if (ret == 0) {
        // 客户端关闭连接
        printf("Connection closed by client: fd=%d\n", fd);
        event_del(g_efd, ev);
        close(fd);
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 没有更多数据可读，继续等待
            event_add(g_efd, EPOLLIN, ev);
        } else {
            perror("recv error");
            event_del(g_efd, ev);
            close(fd);
        }
    }
}

void event_add(int epfd, int events, struct myevent_s *ev) {
    struct epoll_event epv = {0};
    int op;
    
    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    
    epv.events = ev->events = events;
    epv.data.ptr = ev;
    
    if (epoll_ctl(epfd, op, ev->fd, &epv) == -1) {
        perror("epoll_ctl error");
    } else {
        printf("Event %s: fd=%d, events=%s\n", 
               (op == EPOLL_CTL_ADD) ? "added" : "modified",
               ev->fd,
               (events & EPOLLIN) ? "EPOLLIN" : "EPOLLOUT");
    }
}

void cb_accept_conn(int fd, int events, void *arg) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int cfd = accept(fd, (struct sockaddr *)&client_addr, &len);
    
    if (cfd == -1) {
        perror("accept error");
        return;
    }
    
    char ip[INET_ADDRSTRLEN];
    printf("New connection: %s:%d\n", 
           inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)),
           ntohs(client_addr.sin_port));
    
    // 查找空闲事件槽
    int i;
    for (i = 0; i < MAX_EVENTS; i++) {
        if (g_events[i].status == 0) break;
    }
    
    if (i == MAX_EVENTS) {
        fprintf(stderr, "Too many connections\n");
        close(cfd);
        return;
    }
    
    // 设置非阻塞
    int flags = fcntl(cfd, F_GETFL);
    if (flags == -1 || fcntl(cfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl error");
        close(cfd);
        return;
    }
    
    // 初始化事件
    event_set(&g_events[i], cfd, EPOLLIN | EPOLLET, &g_events[i], cb_recv_data);
    event_add(g_efd, EPOLLIN | EPOLLET, &g_events[i]);
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
    
    // 初始化监听事件
    event_set(&g_events[MAX_EVENTS], lfd, EPOLLIN | EPOLLET, &g_events[MAX_EVENTS], cb_accept_conn);
    event_add(g_efd, EPOLLIN | EPOLLET, &g_events[MAX_EVENTS]);
    printf("Server listening on port %d...\n", PORT);
    
    // 事件循环
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(g_efd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }
        
        for (int i = 0; i < nfds; i++) {
            struct myevent_s *ev = events[i].data.ptr;
            
            // 处理错误事件
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                printf("Error event on fd=%d\n", ev->fd);
                event_del(g_efd, ev);
                close(ev->fd);
                continue;
            }
            
            // 调用回调函数
            if (ev->call_back) {
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
        }
    }
    
    close(g_efd);
    return 0;
}