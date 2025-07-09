#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
//实现ls | wc -l
void pipe_test()
{
    int pfd[2];
    int ret = pipe(pfd);
    if(ret == -1) {
        perror("pipe error");
        exit(1);
    }
    //由父进程写，子进程读
    int pid = fork();
    if(pid == 0) {
        //子进程
        close(pfd[1]);//关闭写端
        int newfd = dup2(pfd[0], STDIN_FILENO);
        if(newfd == -1) {
            perror("child process dup2 error");
            exit(1);
        }
        execlp("wc", "wc", "-l", NULL);
        perror("child process execp error");

    } else if(pid > 0) {
        //父进程
        close(pfd[0]);//关闭读
        int newfd = dup2(pfd[1], STDOUT_FILENO);
        if(newfd == -1) {
            perror("parent process dup2 error");
            exit(1);
        }
        execlp("ls", "ls", NULL);
        perror("parent process execp error");
    } else {
        perror("fork error");
        exit(1);
    }


}
//mkfifo test
void mkfifo_test()
{
    char msg[] = "hello world\n";
  
    int fd = open("tem/mypipe", O_WRONLY);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    int ret = write(fd, msg, strlen(msg));
     if(ret == -1) {
        perror("write error");
        exit(1);
    }

    close(fd);
}
int main(int argc, char const *argv[])
{
    //pipe_test();
    mkfifo_test();

    return 0;
}