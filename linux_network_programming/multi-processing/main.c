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

#define IP      "0.0.0.0"
#define PORT    12348
#define BACKLOG 128
#define INET_ADDRLENGTH 16
#define BUF_SIZE 1024

static int count = 0;
//回收子进程的信号处理函数
void handle_CHLD(int sig) {
    pid_t wpid;
    while ((wpid = waitpid(-1, NULL, WNOHANG)) > 0) {
        count--;
        printf("回收子进程：%d  剩余连接数：%d\n ", wpid, count);
        
    }
}

//服务器端程序
//测试：ip和端口写死了
int main(int argc, char  *argv[])
{
      struct sigaction act, oact;
        act.sa_handler = handle_CHLD;
        act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
        sigemptyset(&act.sa_mask);

        if (sigaction(SIGCHLD, &act, &oact) == -1) {
            perror("sigaction error");
            exit(1);
        }

    //创建监听socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("connect error");
        exit(1);
    }
    //常见初始化服务器监听socket地址结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);
    
    int ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bine error");
        exit(1);
    }

    ret = listen(listen_fd, BACKLOG);
    if (ret == -1) {
        perror("listen error");
        exit(1);
    }
    
    struct sockaddr_in client_addr;//用于接收成功建立连接的客户端的地址结构体
    socklen_t cli_addr_len = sizeof(client_addr);//必须传个变量
    //cfd为连接成功后生成用于与客户端通信的socket
    while (1) {
        
      
        //阻塞等待建立连接
        int cfd = accept(listen_fd, (struct sockaddr *)&client_addr, &cli_addr_len);
        if (cfd == -1 ) {
            if(errno == EINTR) {
                printf("accept 被信号打断，已经重新启动\n");
            } else {
                perror("accept error");
                exit(1);
            }
            
        } 
        char dst[INET_ADDRLENGTH];
        memset(dst, '\0', INET_ADDRLENGTH);
        const char *conn_ip = inet_ntop(AF_INET, &client_addr.sin_addr, dst, INET_ADDRLENGTH);
        const int conn_port = ntohs(client_addr.sin_port);
    
        //建立连接成功，创建子进程
        pid_t pid = fork();

        if (pid == 0) {
            close(listen_fd);
            int n, buf[BUF_SIZE];
            while (1) {
                if ((n = read(cfd, buf, BUF_SIZE - 1)) > 0) {
                    //memset(buf, '\0', BUF_SIZE);
                    write(STDOUT_FILENO, buf, n);
                } else if (n == -1 && errno == EWOULDBLOCK) {
                    continue;
                } else if(n == 0){
                    
                    printf("%s:%d已经断开连接\n", conn_ip, conn_port);
                    close(cfd);
                    exit(1);

                } else {
                    perror("read error");
                    exit(1);
                }
                
                
            }
        } else {
            close(cfd);
            count++;
            
            printf("%s, %d 已经成功连接，当前已连接%d个用户\n", conn_ip, conn_port, count);
        }
            
           


    }




   


    return 0;
}