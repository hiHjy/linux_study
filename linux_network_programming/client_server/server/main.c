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


#define IP      "192.168.1.33"
#define PORT    12345
#define BACKLOG 3
#define INET_ADDRLENGTH 16

//服务器端程序
//测试：ip和端口写死了
int main(int argc, char  *argv[])
{
    //创建监听socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("connect error");
        exit(1);
    }
    //常见初始化服务器地址结构体
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
    int cfd = accept(listen_fd, (struct sockaddr *)&client_addr, &cli_addr_len);
    if (cfd == -1) {
        perror("accept error");
        exit(1);
    }
    char dst[INET_ADDRLENGTH];
    memset(dst, '\0', INET_ADDRLENGTH);
    printf("%s, %d\n",
         inet_ntop(AF_INET, &client_addr.sin_addr, dst, INET_ADDRLENGTH),
        ntohs(client_addr.sin_port));
    while(1);

    return 0;
}