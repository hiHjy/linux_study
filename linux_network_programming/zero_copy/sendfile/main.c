#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
int main(int argc, char const *argv[])
{
    int fd_in = open("sogoupinyin_4.2.1.145_amd64.deb", O_RDONLY);
    int fd_out = open("sogoupinyin_4.2.1.145_amd64.deb.copy", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    struct stat f_in_stat;
    fstat(fd_in, &f_in_stat);
    
    sendfile(fd_out, fd_in, 0, f_in_stat.st_size);

    close(fd_in);
    close(fd_out);
    return 0;
}