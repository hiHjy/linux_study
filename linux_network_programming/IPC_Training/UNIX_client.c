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
#define C_PATH "c.socket"
#define S_PATH "s.socket"
//本地套接字
int main(int argc, char const *argv[])
{
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un c_addr;
    memset(&c_addr, 0, sizeof(c_addr));
    strcpy(c_addr.sun_path, C_PATH); 
    c_addr.sun_family = AF_UNIX;

    unlink(C_PATH);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(C_PATH);
    int ret = bind(cfd, (struct sockaddr *)&c_addr, len);
    assert(ret == 0 && "bind error");

/*---------------------------------------------------------------------------------*/
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un s_addr;
    memset(&s_addr, 0, sizeof(s_addr));
    strcpy(s_addr.sun_path, S_PATH); 
    s_addr.sun_family = AF_UNIX;
    len = offsetof(struct sockaddr_un, sun_path) + strlen(S_PATH);

    int buf[1024];

    ret = connect(cfd, (struct sockaddr *)&s_addr, len);
    if (ret == -1) {
        perror("connect error");
        exit(1);
    }
    

    while (1) {
        
        ret = read(STDIN_FILENO, buf, 1024);
        write(cfd, buf, ret);
      
    }
    return 0;
}