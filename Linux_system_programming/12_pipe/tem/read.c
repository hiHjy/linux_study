#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
    char buf[1024];
    int ret;
    if(ret == -1) {
        perror("mkfifo error");
        exit(1);
    }
    int fd = open("./mypipe", O_RDONLY);
     if(fd == -1) {
        perror("open error");
        exit(1);
    }
    ret = read(fd, buf, sizeof(buf));
    if(ret == -1) {
        perror("read error");
        exit(1);
    }
    write(STDOUT_FILENO, buf,ret);
    close(fd);
    return 0;
}
