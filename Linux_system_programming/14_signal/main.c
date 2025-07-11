#define _POSIX_C_SOURCE 200809L  // 我要启用 2008 版 POSIX API
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
/*
                                信号集操作函数练习
    1,将2号SIGINT信号加入屏蔽信号集
    2，用sigpending获取待处理信号集
    3，用死循环+sigismember函数输出待处理信号集1-31号信号的值
    4，ctrl + c 发送 SIGINT信号给当前进程
    5，发现待处理信号集的2号信号变为1，但是进程并没有终止，说明屏蔽信号集设置成功

*/

void procmask_test()
{
    /*
        set:        自定义信号屏蔽集 
        old_set:    保存原来存在于PCB中的信号屏蔽集
        pending_set: 待处理信号集

    */
    sigset_t set, old_set, pending_set;
    sigemptyset(&set);
    //1,将2号SIGINT信号加入屏蔽信号集
    sigaddset(&set, SIGINT);
    int ret = sigprocmask(SIG_BLOCK, &set, &old_set);
    if (ret == -1) {
        perror("sigprocmask error");
        exit(1);
    }
    
    if (ret == -1) {
        perror("sigpending error");
        exit(1);
    }
    int i = 1;
    while (1) {
        sigpending(&pending_set);
        for (i = 1; i < 32; i++) {
            char ret = sigismember(&pending_set, i);
            sleep(1);
            if (ret) {
                putchar('1');

            } else {
                putchar('0');
            }
            fflush(stdout);
            
        }
        putchar('\n');
       
    }
    

    // sigset_t set1;
    // sigemptyset(&set1);
    // sigpending(&set1);
    //printf("%ld\n", set1.__val);
    
}

int main(int argc, char const *argv[])
{
    procmask_test();


    return 0;
}