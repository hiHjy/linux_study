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
#include <bits/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>


static bool stop = false;
void handler_term(int sig) {
    stop = true;
}
//测试listen的backlog
void listen_test(char *ip_address, char *port, char *backlog) {
    struct sigaction act;
    int ret;
    act.sa_handler = handler_term;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    ret = sigaction(SIGTERM, &act, NULL);
    assert(ret != -1);

    /*----创建socket---*/
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock != -1); 
    /*----命名socket---*/
    struct sockaddr_in addr; //地址结构 ip + 端口
    addr.sin_family = AF_INET;
    ret = inet_aton(ip_address, &addr.sin_addr);//ip地址
    assert(ret != 0);
    addr.sin_port = htons( atoi(port));//端口


    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    assert(ret != -1);
    listen(sock, atoi(backlog));

    while (!stop) {
        sleep(1);
    }
    close(sock);
    
}

void accept_test(char *ip_address, char *port, char *backlog) {

    int ret;

    /*----创建socket---*/
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock != -1); 

    /*----命名监听socket---*/
    struct sockaddr_in addr; //地址结构 ip + 端口
    addr.sin_family = AF_INET;  
    ret = inet_aton(ip_address, &addr.sin_addr);//ip地址
    assert(ret != 0);
    addr.sin_port = htons( atoi(port));//端口
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    assert(ret != -1);
   /*-------开始监听-----------------------*/ 
    listen(sock, atoi(backlog));
    /*-----------在20秒内关闭已经和成功成功建立连接的------------*/
    sleep(40);
    struct sockaddr_in client_sockaddr;
    socklen_t client_addrlen = sizeof(client_sockaddr);
     
    int confd = accept(sock, (struct sockaddr *)&client_sockaddr, &client_addrlen);
   
    if (confd < 0) {
        perror("accept error");
        exit(1);
    } else {
        char ip[16];
        printf("%s : %d\n", inet_ntop(AF_INET,
            &client_sockaddr.sin_addr, ip,
             sizeof(client_sockaddr)), ntohs(client_sockaddr.sin_port));
    }
    

    close(sock);


}






int main(int argc, char  *argv[])
{
    
    if (argc < 4) {
        printf("usage: %s ip_address port backlog\n", basename(argv[0]));
        exit(1);
    }    
    //listen_test(argv[1], argv[2], argv[3]);
    accept_test(argv[1], argv[2], argv[3]);

    return 0;
}