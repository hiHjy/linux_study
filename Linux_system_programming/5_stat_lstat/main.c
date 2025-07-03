#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
 #include <sys/stat.h>
/**
 * int stat(const char *pathname, struct stat *statbuf);
 * int lstat(const char *pathname, struct stat *statbuf);
 * @brief 查看文件属性
 * 
 * @param pathname  有效的文件路径
 * @param statbuf   存有文件属性的结构体(传入传出参数，man 手册中有具体成员)
 * 
 * @retval success 0
 *         error   -1 errno      
 * 
 *         S_ISREG(m)  is it a regular file?

           S_ISDIR(m)  directory?
 
           S_ISCHR(m)  character device?

           S_ISBLK(m)  block device?

           S_ISFIFO(m) FIFO (named pipe)?

           S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.) 
                stat()(会穿透符号连接)
                lstat()(不会穿透符号连接)

           S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)


 st_mode 两块信息
 ┌───────────────┬─────────────────────────────┐
 │  高位 (4 bits)│   文件类型 (S_IFxxx)          │
 ├───────────────┼─────────────────────────────┤
│  低位 (12 bits)│   9 个 rwx 权限 + 3 个特殊位  │
└───────────────┴─────────────────────────────┘

 */
int main(int argc, char const *argv[])
{
    struct stat ss;
    int ret = lstat(argv[1], &ss);
    if(ret == -1) {
        perror("stat error");
        exit(1);
    }
    int st_mode = ss.st_mode;
    if(S_ISREG(st_mode)) {
        printf("f\n");
    } 

     if(S_ISDIR(st_mode)) {
        printf("d\n");
    } 

   


    return 0;
}