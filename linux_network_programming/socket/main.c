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

int main(int argc, char  *argv[])
{
    
    if (argc < 3) {
        printf("usage: %s ip_address port backlog\n", basename(argv[0]));
    }    
    listen_test(argv[1], argv[2], argv[3]);


    return 0;
}