#include <bits/types/sigset_t.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
/*************************利用信号回收子进程**************************************/
 /*
    SIGCHLD
        产生条件：子进程终止时；
                子进程收到SIGSTOP信号停止时；
                子进程处于停止态，接受到SIGCONT唤醒时；
    
    

 
 */   
 void sig_handler() {
    int wpid;
    while ((wpid = wait(NULL)) > 0) {
        printf("------%d已回收\n", wpid);
    }

 }
int main(int argc, char const *argv[])
{
    int i,pid;
    sigset_t set, oset;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, &oset);

    for (i = 0; i < 10; i++) {
        if ((pid = fork()) == 0) {
            if (pid == -1) {
                perror("fork error");
                exit(1);
            } 
            break;
        }
    }    

    if (pid > 0) {
        struct sigaction act, oact;    
        sleep(5);
        act.sa_handler = sig_handler;
        
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;        
        
        int ret = sigaction(SIGCHLD, &act, NULL);
        if (ret == -1) {
                perror("sigaction error");
                exit(1);
            } 
        sigprocmask(SIG_UNBLOCK, &set, NULL); 
        
    } else if (pid == 0) {
        printf("%d\n", getpid());
    } 

    return 0;
}