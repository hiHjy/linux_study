#include <asm-generic/socket.h>
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
#include <stdatomic.h>


#define IP      "0.0.0.0"
#define PORT    12345
#define BACKLOG 128
#define INET_ADDRLENGTH 16
#define BUF_SIZE 1024
static int  NUM = 0;
volatile sig_atomic_t stop = 0;
static int lfd;
int i = 0;
int cfd_arr[1024];
struct client_info {
    int cfd;
    char ip[INET_ADDRLENGTH];
    int port;
};



void signal_handle(int sig) {
    shutdown(lfd, SHUT_RD);
    close(lfd);
    stop = true;
    
    int n;
    for (n = 0; n < i; ++n) {
        shutdown(cfd_arr[n], SHUT_RD);
    }
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *mt_fun(void *arg) {
    char buf[BUF_SIZE];
    struct client_info *ci = (struct client_info *)arg;
    
    int count;
    
    
    while (!stop) {
        count = read(ci->cfd, buf, BUF_SIZE);
        if (count > 0) {
            write(STDOUT_FILENO, buf,count);
        } else if (count == 0) {
            char *msg = "服务器已经关闭！\n";
            write(ci -> cfd, msg, strlen(msg));
            break;
        } else {
                perror("read");
                
                break;
          
          
            
        }

    }
    
      
        pthread_mutex_lock(&mutex);
        NUM--;
        
        printf("%s:%d已经断开连接,当前用户数量：%d\n", ci->ip, ci->port, NUM);
        pthread_mutex_unlock(&mutex);
        close(ci->cfd);
        free(arg);
        pthread_exit(NULL);


    
    return NULL;
}


//多线程服务器
int main(int argc, char  *argv[])
{
    /*-----注册信号-----*/
    struct sigaction act;
   
    act.sa_handler = signal_handle;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
   
    //创建监听socket
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    // //设置端口复用
    // int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)(intptr_t)1, sizeof(int));
   
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
     if (ret == -1) {
        perror("setsockopt");
    } 
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);
    /*监听socket创建完成*/
    if (bind(lfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind error");
        exit(1);
    }    
    
    //启动监听
    if (listen(lfd, BACKLOG) == -1) {
        perror("listen error");
        exit(1);
    }

    
    while (!stop) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int cfd;
        //接受连接
        if ((cfd = accept(lfd, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EINVAL && stop){
                perror("accept");
                break;
                
            } else {
                perror("accept");
                break;

            }
           
        }
       
        
        /*-----获取成功连接客户端的 cfd ip：port-------*/
        struct client_info *ci = malloc(sizeof(struct client_info));
        
        ci->cfd = cfd;
        inet_ntop(AF_INET, &client_addr.sin_addr, ci->ip, INET_ADDRLENGTH);
        ci->port = ntohs(client_addr.sin_port);
       
        cfd_arr[i++] = cfd;
        pthread_mutex_lock(&mutex);
        
        NUM++;
        printf("用户*%s:%d*已成功连接，目前用户数：%d\n", ci->ip, ci->port, NUM);
        pthread_mutex_unlock(&mutex);

        pthread_t tid;
        
        if (pthread_create(&tid, NULL, mt_fun, ci) != 0) {
            perror("create error");
            close(cfd);
            free(ci);
            continue;
        }

        pthread_detach(tid);
     

    }

    printf("服务器正在关闭...\n");
    close(lfd);
    while (NUM > 0) {
        sleep(1); //如果还有其他客户端存在，先等其他客户端关闭
    }
    printf("服务器已经彻底关闭！\n");
 
    return 0;
}