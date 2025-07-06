#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *   int execl(const char *pathname, const char *arg, ... # (char  *) NULL);
 *   
 *   int execlp(const char *file, const char *arg, ... # (char  *) NULL);    
 *                                                           #在PATH环境变量的命令
 *  @brief 将当前进程的.text，。data替换为要加载程序的.text,.data,
 *         然后让进程从新的.text的第一条指令开始执行，但进程id不变，换核不换壳。
 *          execlp("./main", “./main”, argv[1], argv[2], NULL);
 *  @param pathname
 *  @param arg 从argv[0]开始，必须以NULL结束
 * 
 * 
 * 
 * 
 */
//将ps aux 的结果打印到文件中
void print_ps_to_file()
{
    int pid;
    pid = fork();
    if(pid == -1) {
        perror("fork error");
    }
    if( pid == 0) {
        int fd = open("./test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
        if(fd == -1) {
            perror("open error");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);

        execlp("ps", "ps", "aux", NULL);
       perror("execl error");
       exit(1);

    } else if(pid > 0) {

       
    } 

}
int main(int argc, char const *argv[])
{
    print_ps_to_file();
    


    return 0;
}