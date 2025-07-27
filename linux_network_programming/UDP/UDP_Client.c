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
#define IP "127.0.0.1"
int main(int argc, char const *argv[])
{
    int cfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buf[BUFSIZ];

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, IP, &server_addr.sin_addr); 

  
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    int ret;
    while (1) {
        
       
        int ret = read(STDIN_FILENO, buf, BUFSIZ);
    
        if ((ret = sendto(cfd, buf, ret, 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in))) < 0) {
            perror("sendto error");
            exit(-1);
        }
        
    }
    

    return 0;
}