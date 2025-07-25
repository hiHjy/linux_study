#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
/**
 *  pid_t wait(int *wstatus);
 *  @brief  1，阻塞等待子进程退出
 *          2，回收子进程在内核中残存的资源
 *          3，获取子进程退出状态（退出原因）
 * @param wstatus   进程退出状态（可传入NULL表示不关心子进程怎么死的）
 *                  WIFEXITED(wstatus)      是否正常终止？ 真：正常终止 
 *                  WEXITSTATUS(wstatus)    正常终止，返回子进程退出的值（return的值）
 * 
 *                  WIFSIGNALED(wstatus)    是否由于信号导致的终止
 *                  WTERMSIG(wstatus)       确认信号是信号导致的终止，返回造成导致终止的信号
 *               
 *                                          
 *                        
 * 
 * @retval  success: pid(回收的pid)
 *          error： -1
 * 
 * 
 * 
 */

/*------------------------------------------------------------------------------------------*/

/**
 *  pid_t waitpid(pid_t pid, int *wstatus, int options);
 *  @brief  wait升级版
 *  @param  pid     -1  表示回收任意子进程（相当于wait）
 *                  >0  回收指定进程 
 *                   *0  回收和waitpid一个组的所有子进程
 *                 *<-1  回收指定进程组的任意子进程（-1552：回收1552组的任意子进程）
 * 
 * 
 *                  A（pgid = A）          # A是父进程，BCD是A创建的子进程，
 *                  ├── B（pgid = A）      # BCD默认属于A的进程组所有进程默认
 *                  ├── C（pgid = A）      # 属于 A 的 进程组，group id 为 A 的 pid。
 *                  │   └── E（pgid = A）  # C创建的E，E默认继承C的进程组属于A组，即 pgid=A。
 *                  └── D（pgid = A）       #setpgid()/setsid()可以改变组id
 * 
 *  @param  wstatus 
 *  @param  options WNOHANG
 *  
 *  @retval >0: 表示成功回收子进程的pid
 *          0： options设置WNOHANG,无僵尸进程可以回收，但是有子进程存活
 *          -1：  回收失败，errno没有更多子进程可等待，errno = ECHILD（**正常结束**）  
 *               或真正出错(errno 其余值)
 * 
 * 
 * 
 * 
 * 
 * 
 */



//wait
int wait_test() 
{
    int  pid;
    pid = fork();
    if( pid == 0) {
        printf("child process pid:%d\n", getpid());
        sleep(10);
        return 1;
     
    } else if(pid > 0) {
        int wstatus;

        int w_pid = wait(&wstatus);
        if(WIFEXITED(wstatus)){
            //如果正常死亡

            int ret;
            if((ret = WEXITSTATUS(wstatus))) {
                printf("子进程:%d正常退出，return值为：%d\n", w_pid, ret);
            }
        } else if(WIFSIGNALED(wstatus)) {
                //如果死于信号
                int ret = WTERMSIG(wstatus);
                printf("子进程:%d死于信号，信号编号为：%d\n", w_pid, ret);
        } else {
            perror("wait error");
            exit(1);
        }
       
        return 1;
    } else {
        perror("fork error");
        exit(-1);
    }
    
}
//waitpid回收多个子进程
int waitpid_test()
{
    int pid,i;
    for(i = 0; i < 5; ++i) {
        pid = fork();
        if(pid == 0)
            break;
        
    }
    
    if(pid > 0) {
        int wpid;
        while( (wpid = waitpid(-1, NULL, 0)) > 0) {
            printf("回收子进程：%d\n", wpid);
            
        }
        return 1;
    } else if(pid == 0){
        printf("%d\n", getpid());
        return 1;
    } else {
        perror("wait error");
        exit(-1);
    }



}
//回收第三个创建的进程
int waitpid_test2()
{
    int pid, i, tem_pid ;
    for(i = 0; i < 5; ++i) {
        pid = fork();
       
        if(pid == 0){
           
            break;
        }
         if(i == 2) {
                tem_pid = pid;
                printf("要回收的进程为:%d\n", pid);
              
            } 
        
    }
    
    if(pid > 0) {
       
        int wpid;
        sleep(1);
        while( (wpid = waitpid(tem_pid, NULL, 0)) > 0) {
            printf("回收子进程：%d\n", wpid);
            
        }
        return 1;
    } else if(pid == 0){
        printf("%d\n", getpid());
        return 1;
    } else {
        perror("wait error");
        exit(-1);
    }



}
int main(int argc, char const *argv[])
{
    waitpid_test2();
    

    return 0;
}