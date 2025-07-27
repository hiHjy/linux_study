#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#define PORT 12344
int main(int argc, char const *argv[])
{
    int lfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buf[BUFSIZ];

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(lfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0) {
        perror("bind error");
        exit(-1);
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    int ret;
    while (1) {
        
        if ((ret = recvfrom(lfd, buf, BUFSIZ, 0, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("recvfrom error");
            exit(-1);
        } else if (ret == 0) {
            printf("对端是流socket,或udp数据包大数据为0\n");
        } else {
           ret = write(STDOUT_FILENO, buf, ret);
            assert(ret >= 0);
        }
        
    }
    

    return 0;
}