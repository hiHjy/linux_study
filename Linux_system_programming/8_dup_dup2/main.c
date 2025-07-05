#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *  int dup(int oldfd);
 *  @brief 复制一个文件描述符 
 *  @param oldfd 原文件描述符
 * 
 *  @retval success: new fd(是复制的原有文件描述符)
 *          error:   -1,errno
 *  
 * 
 */

 void dup_test(const char *path)
{
    int fd = open(path, O_RDWR);

    int new_fd = dup(fd);
    printf("fd=%d,new_fd=%d\n", fd, new_fd);
    
    close(fd);
    close(new_fd);
}
/**
 *  int dup2(int oldfd, int newfd);
 *  @param oldfd 文件描述符
 *  @param newfd 使得newfd->oldfd            
 *  @retval success: newfd
 *          error:   -1,errno
 *  @note   1，如果newfd是之前open出来的，那么会默认close（newfd），然后newfd->oldfd
 *          2，若newfd==oldfd 就不会close（newfd）而会返回newfd
 *          3，本质上：newfd 和 oldfd 指向同一个 open file description,
 *             因此共享文件偏移量、读写模式、锁等。
 *          4，默认新描述符不带 FD_CLOEXEC 标志；
 *          如果需要在 exec() 后关闭，应再用 fcntl(newfd, F_SETFD, FD_CLOEXEC)，
 *          或在更新版本使用 dup3(oldfd, newfd, O_CLOEXEC) 一步完成。
 */
void dup2_test(const char *path)
{
    
    int fd = open(path, O_RDWR);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    
    int ret = dup2(STDOUT_FILENO, fd); 
   
    if(ret == -1) {
        perror("ret error");
        exit(1);
    }
    write(fd, "dfdsfadfadfdasfdasfadsfafafafafaf\n",
        strlen("dfdsfadfadfdasfdasfadsfafafafafaf\n"));
    
    
    
}

int main(int argc, char const *argv[])
{
    //dup2_test(argv[1]);
    int fd = open("test.txt", O_RDWR | O_APPEND);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    
    int newfd = fcntl(fd, F_DUPFD, 99);
    printf("newfd=%d\n", newfd);
    close(fd);
    write(newfd, "12345678\n", strlen("12345678\n"));

    return 0;
}