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
#include <pthread.h>
#include <stdint.h>

void  *thread_handle(void *arg) {
    char buf[1024];
   
    int cfd = (int)(intptr_t)arg;
    int ret;

    while ((ret = read(cfd, buf, sizeof(buf))) > 0) {
                           
        write(STDOUT_FILENO, buf, ret);
    }   

    close(cfd);
    exit(0);
    
}



//客户端程序
int main(int argc, char  *argv[])
{
    
    if (argc < 3) {
        printf("usage: %s ip_address port\n", basename(argv[0]));
        exit(1);
    }    
    int buf[1024], tmp;
    //将传入的port字符串转为整数
    int port = atoi(argv[2]);
    //创建并初始化要连接服务器的地址结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    //创建客户端socket
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    socklen_t len = sizeof(server_addr);
    int ret = connect(client_sockfd, (struct sockaddr *)&server_addr, len);
    if (ret == -1) {
        perror("connect error");
        exit(1);
    }
    printf("连接成功\n");
    pthread_t tid;
    ret = pthread_create(&tid, NULL, thread_handle, (void *)(intptr_t)client_sockfd);
    if (ret != 0) {
        perror("pthread_create error");
        exit(1);
    }


    
    while (1) {
         
        int ret = read(STDIN_FILENO, buf, 1024);
        ret = write(client_sockfd, buf, ret);
        if (ret == -1) {
            perror("write error");
            break;
        }
        
		
	  
	
    }
  
    return 0;
}