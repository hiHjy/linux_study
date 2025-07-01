#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
/**
 * @brief 打开一个文件，并返回文件描述符
 * int open(const char *pathname, int flags);
 * int open(const char *pathname, int flags, mode_t mode);
 * @param pathname 文件路径
 * @param flags 打开方式 
 *              O_RDONLY(只读)  
 *              O_RDONLY | O_CREAT() 
 *              O_WRONLY(只写)
 *              O_RDWR
 *              O_TRUNC 把文件清0 O_RDONLY | O_CREAT | O_TRUNC
 *                  存在：只读方式打开，并把文件清空
 *                  不存在：创建
 * @param mode 打开一个文件若文件不存在则创建它的初始权限0xxx(八进制) 0644
 *              文件权限 = mode & ~umask
 * @retval 成功 fd 失败 -1 set errno
 * @note 需要头文件<sys/types.h>  <sys/stat.h> <fcntl.h> ，其中<sys/types.h>  <sys/stat.h>
 *       包括在<unistd.h>中
 */
int main(int argc, char const *argv[])
{
    int fd1 = open("./test.txt", O_RDONLY | O_CREAT | O_TRUNC, 0644);
    //打开不存在的文件
    int fd2 = open("./test1.txt", O_RDONLY);
    //fd = -1 set errno use strerr(errno) to check msg
    //strerror string.h
    printf("fd2 = %d,errno = %d, errno msg :%s\n", fd2, errno, strerror(errno));
    return 0;
}