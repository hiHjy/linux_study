#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
 #include <sys/stat.h>

 /*复制权限*/
 int main(int argc, char const *argv[])
 {
    struct stat sb;
    int ret = stat(argv[1], &sb);
    if(ret == -1) {
        perror("stat error");
        exit(1);
    }   

    int mode = sb.st_mode & 07777;
    chmod(argv[2], mode);
    return 0;
 }
 