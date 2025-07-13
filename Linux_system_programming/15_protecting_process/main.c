#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
//fork 一个子进程，并使其创建一个新会话。查看进程组id和，会话id的变化
void session_test() {
    pid_t pid = fork();
    if (pid > 0) {
        sleep(6);
    } else {
        printf("创建会话前进程id:%d，进程组id：%d，会话id：%d\n", getpid(),
                                getpgid(0), getsid(getpid()));
        sleep(5);
        int ret = setsid();
        if (ret == -1) {
            perror("setsid error");
            exit(1);
        }
         printf("创建会话后进程id%d，进程组id：%d，会话id：%d\n", getpid(),
                                getpgid(0), getsid(getpid()));

    }    
}

//创建守护进程
void create_daemon() {
    int pid;
    

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0)
    {
        int ret = setsid();
        if (ret == -1) {
            perror("setsid error");
            exit(1);
        }
        char *path = getenv("HOME");//取环境变量 /home/hjy
        if (!path) {
            perror("get home error");
            exit(1);
        } 

        ret = chdir(path);  //切换工作目录到/home/hjy
        if (ret == -1) {
            perror("chdir error");
            exit(1);
        }
        ret = open("/dev/null", O_RDWR);
         if (ret == -1) {
            perror("open error");
            exit(1);
        }

        dup2(ret, STDOUT_FILENO);//重定向0 1 2 到 /dev/null
        dup2(ret, STDIN_FILENO);
        dup2(ret, STDERR_FILENO);
        close(ret);

        
        FILE *fp = fopen("daemon.log", "a+");
        if (!fp) {
            perror("fopen error");
            exit(1);
        }
        while (1) {
            fprintf(fp, "-------守护进程正在执行------\n" );
            fflush(fp); //全缓冲要手动fflush（fp）
            sleep(1);
        }
    }
}

int main(int argc, char const *argv[])
{
    //session_test();
    create_daemon();

    return 0;
}