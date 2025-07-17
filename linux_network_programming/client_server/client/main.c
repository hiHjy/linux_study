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




//客户端程序
int main(int argc, char  *argv[])
{
    
    if (argc < 3) {
        printf("usage: %s ip_address port\n", basename(argv[0]));
        exit(1);
    }    
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
    ret = send(client_sockfd,"这是常规数据1", sizeof("这是常规数据1"),0);
   
    ret = send(client_sockfd,"-----", sizeof("-----"),MSG_OOB);
    ret = send(client_sockfd,"这是常规数据2", sizeof("这是常规数据2"),0);
    if (ret == -1) {
        perror("send error");
        exit(1);
    }
    while(1);
  

    return 0;
}