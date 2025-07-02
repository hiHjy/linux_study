#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *  int fcntl(int fd, int cmd, ... );
 *  @brief  fcntl有三大作用，本次在学习其中的一种 ：
 *              修改文件属性(不需要打开文件)
 *              flags = fcntl(int fd, F_GETFL); 
 *              fcntl(fd, F_SETFL, int flags );
 *  @param  fd
 *  @param  cmd     F_GETFL    获取文件属性
 *                  F_SETFL    设置文件属性   
 * 
 *  @param  flags   
 * 
 *  @retval flags 文件属性
 *                       
 *           
 */

 /*本次测试fcntl修改read读/dev/tty 修改为非阻塞*/
int main(int argc, char const *argv[])
{ 
    int tty_fd = open("/dev/tty", O_RDWR | O_NONBLOCK);
    char buf[10];
    int time_count = 0;
    if(tty_fd == -1) {
        perror("open tty error");
        exit(1);
    }
    int count;
try_again:
         count = read(tty_fd, buf, 10);
    if(count == -1 && errno == EAGAIN) {
        //非阻塞读到空
        printf("非阻塞读到空\n");
        sleep(2);
        time_count++;
        if(time_count == 5) {
            //非阻塞read 读了5次后修改文件属性为阻塞
            int flags = fcntl(tty_fd, F_GETFL);
            fcntl(tty_fd, F_SETFL, flags & ~O_NONBLOCK );
        }
        goto try_again;
    } else if(count == -1) {
        //读错误
        perror("read tty error");
    }
    write(STDOUT_FILENO, buf, count);

    return 0;
}