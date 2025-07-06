#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 * pid_t fork(void):
 * @brief   创建子进程
 * @retval  0 子进程
 *          >0 子进程的进程id
 *          -1 errno
 * @note： 1，读时共享写时复制（全局变量 ）
 *           父子进程共享也共享fd和mmap
 *        2，刚fork后，data段，text段，堆，栈，环境变量，
 *          全局变量，进程工作目录位置，信号处理方式
 */
int main(int argc, char const *argv[])
{
    int i = 0, pid, val = 999;
    pid = fork();
    if( pid == 0) {
        val = 111;
       
     
    } else if(pid > 0) {

        sleep(1);
        printf("parent process:%d\n",val);
    } 
    

    return 0;
}