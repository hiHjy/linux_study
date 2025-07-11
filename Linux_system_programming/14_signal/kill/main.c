#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
/**
 *  int kill(pid_t pid, int sig);
 *  @brief 给指定进程发送信号
 *  @param  pid >0:发送信号给指定进程
 *              =0:发送信号给调用kill函数进程所属同一进程组的所有进程
 *              <0:发给|pid|进程组
 *              -1：发送给在系统中进程有权限发送的所有进程
 *                  权限保护：root可以发给任意用户，普通用户不能给系统用户发送信号，
 *                          普通用户不能发给root用户，普通用户不能发给其他普通用
 *                          户，普通用户只能向自己创建的进程发送信号。
 *  @param sig  信号
 *  
 * 
 */
//循环创建5个子进程，父进程用kill终止所有子进程
void kill_test()
{
    int pid[5], i;
    for(i = 0; i < 5; ++i) {
        pid[i] = fork();
        if(pid[i] == -1) {
            perror("perror error");
            exit(1);
        }
        
        if(pid[i] == 0)
            break;
    }

    if (pid[i] == 0) {
        //子
        printf("%d\n", getpid());
        while(1);
    } else {
        sleep(3);
        //父
        // for(i = 0; i < 5; ++i) {
        //     int ret = kill(pid[i], SIGKILL);
        //     if(ret == -1) {
        //         perror("kill error");
        //         exit(1);
        //     }
        // }
        
        int wpid, wstatus;

        while((wpid = waitpid(0, &wstatus, 0)) > 0) {
            if( WIFEXITED(wstatus)) {
                printf("正常退出%d\n", wpid);
            } else if(WIFSIGNALED(wstatus)){
                printf("进程%d信号编号为%d杀死\n", wpid, WTERMSIG(wstatus));
            }
        }
    }

}

int main(int argc, char const *argv[])
{
    kill_test();    


    return 0;
}