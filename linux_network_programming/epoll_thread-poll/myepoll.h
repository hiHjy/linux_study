#ifndef __MYEPOLL_H
#define __MYEPOLL_H
#include "thread_pool.h"
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



// 前置声明
void event_set(struct myevent_s *ev,int fd, int events, void *arg, 
               void (*call_back)(int, int, void *));
void event_add(int epfd, int events, struct myevent_s *ev);
void event_del(int epfd, struct myevent_s *ev);
void cb_accept_conn(int fd, int events, void *arg);
void cb_recv_data(int fd, int events, void *arg);
void cb_send_data(int fd, int events, void *arg);
void myepoll_init(int epfd, struct threadpool_t *arg);
#endif