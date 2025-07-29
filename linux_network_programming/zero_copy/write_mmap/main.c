#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#define CHUNK_SIZE (16 * 1024 * 1024)
int main(int argc, char const *argv[])
{
    int fd_in = open("sogoupinyin_4.2.1.145_amd64.deb", O_RDONLY);
    int fd_out = open("sogoupinyin_4.2.1.145_amd64.deb.copy", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    
    struct stat f_in_stat;
    fstat(fd_in, &f_in_stat);
    char * addr = mmap(NULL, f_in_stat.st_size, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (!addr) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
    int offset;
    for (offset = 0; offset < f_in_stat.st_size; offset += CHUNK_SIZE) {
        int  len = offset + CHUNK_SIZE > f_in_stat.st_size ? f_in_stat.st_size - offset : CHUNK_SIZE;
        write(fd_out, addr + offset, len);
    }

    close(fd_in);
    munmap(addr, f_in_stat.st_size);
    close(fd_out);
    return 0;
}