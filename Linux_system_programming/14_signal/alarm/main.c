#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *  unsigned int alarm(unsigned int seconds);
 *  @brief  指定seconds秒后，内核给当前进程发送SIGALRM信号。（默认终止）
 *  
 *  @retval 返回还剩下几秒调用alarm函数
 * 
 * 
 *  @note   #每个定时器有且只有一个定时器
 *          #反复调用alarm会重置,并且返回值为上次调用剩余的时间
 *          #alarm（0）取消定时
 *          #使用time命令查看程序运行时间
 *              实际运行时间 = 系统时间 + 用户时间 + 等待时间
 *              程序运行的瓶颈在于IO，优化程序，首选优化io
 * 
 * 
 */
//测试我的计算机一秒能数几个数
void alarm_test()
{   
    int i = 1;
    alarm(1);
    while (1) {
        printf("%d\n", ++i);
    }
}

int main(int argc, char const *argv[])
{
    
    alarm_test();

    return 0;
}