#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
//mmap test
void mmap_test()
{
    int fd = open("./mmap_file", O_RDWR | O_CREAT | O_TRUNC , 0664);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    int ret = lseek(fd, 1023, SEEK_END);
    if(ret == -1) {
        perror("lseek error");
        exit(1);
    }

    ret = write(fd, "\0", 1);
    if(ret == -1) {
        perror("write error");
        exit(1);
    }

    int length = lseek(fd, 0, SEEK_END); 
    char * m = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    strcpy(m, "qweqweqwe");
    printf("%s\n", m);
    ret = munmap(m, length);
     if(ret == -1) {
        perror("write error");
        exit(1);
    }
    close(fd);
    
}
//无血缘关系进程间利用mmap通信（写端），读端在read
void mmap_test1()
{
    int fd = open("./mmap_file", O_RDWR | O_CREAT | O_TRUNC , 0664);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    int ret = lseek(fd, 1023, SEEK_END);
    if(ret == -1) {
        perror("lseek error");
        exit(1);
    }

    ret = write(fd, "\0", 1);
    if(ret == -1) {
        perror("write error");
        exit(1);
    }

    int length = lseek(fd, 0, SEEK_END); 
    char * m = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);
    while(1) {
        strcpy(m, "A");
        sleep(2);
    }
    
    ret = munmap(m, length);
     if(ret == -1) {
        perror("write error");
        exit(1);
    }
    



}
int main(int argc, char const *argv[])
{
    
    mmap_test1();

    return 0;
}