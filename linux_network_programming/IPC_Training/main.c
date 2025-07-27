#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <assert.h>
#define S_PATH "s.socket"
//本地套接字
int main(int argc, char const *argv[])
{
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un s_addr;
    
    strcpy(s_addr.sun_path, S_PATH); 
    s_addr.sun_family = AF_UNIX;
    unlink(S_PATH);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(S_PATH);
    int ret = bind(sfd, (struct sockaddr *)&s_addr, len);
    assert(ret == 0 && "bind error\n");
    listen(sfd, 3);
    char buf[1024];
    while (1) {
        struct sockaddr_un client_addr;
        socklen_t client_addr_len = sizeof(struct sockaddr_un);
        int cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
        printf("filename:%s\n", client_addr.sun_path);
        int ret;
        
        while ((ret = read(cfd, buf, sizeof(buf))) > 0) {

            if (ret == 0) {
                printf("对端关闭\n");
            } else if (ret > 0) {
                printf("ret:%d\n", ret);
                write(STDOUT_FILENO, buf, ret);
            } else {
                perror("read error");
                exit(-1);
            }
        }
    }
    return 0;
}