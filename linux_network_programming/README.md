# linux网络编程基础

## 1 主机字节序和网络字节序
> - **小端字节序**，高存高，低存低
> - **大端字节序**，高存低，低存高
> - 现代pc绝大部分都是小端，因此小端字节序被称为**主机字节序**
> - 网络上采用大端，所以大端又称**网络字节序**

- **网络编程涉及转换格式的问题**
> (1) 如果通过main函数的`argv[]`中接受的参数,因为各种参数一般都是整数，所以第一步需要将字符串转为整数

```c
    /*
   int atoi(const char *nptr);
    @brief 将传入的port字符串转为整数，这里用作将C字符串表示的端口转为整数

    */
    int port = atoi(argv[2]);

```    
> (2) socket上的的地址结构体`struct sockaddr_in`是要用于网络通信的，所`struct sockaddr_in`上的所有参数都应该用网络字节序,而若是要读取结构体的数据到本地使用就要用本地字节序


```c
        
      /*                  IP   

         int inet_pton(int af, const char *src, void *dst);

            @brief          主机字节序的ip字符串转网络字节序ip整数
            @param af       AF_INET AF_INET6 #ipv4 ipv6
            @param src      要转换成网络字节序的本地字符串
            @param dst      给struct sockaddr_in 类型的ip赋值（传指针） 
     */
            struct sockaddr_in server_addr;
            inet_pton(AF_INET, IP, &server_addr.sin_addr);

    
        /*  
          const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

            @brief          网络字节序整数ip转主机字节序字符串的点分十进制ip
            @param af       AF_INET AF_INET6 #ipv4 ipv6
            @param src      要转换成本地字符串的的网络字节序整数ip（指针）
            @param dst      指定要存字符串ip的buf，一般定义一个char ip[16];
            @retval const char* 类型的指针。存的就是dst的值，他们就是ip[16]的首地址
       */

        struct client_info {
            int cfd;
            char ip[INET_ADDRLENGTH];
            int port;
        };

        struct client_info *ci = malloc(sizeof(struct client_info));
        inet_ptread(cfd, buf, BUF_SIZE - 1on(AF_INET, argv[1], &server_addr.sin_addr);

        /*                  PORT

            uint16_t htons(uint16_t hostshort); #端口 主机转网络
            uint16_t ntohs(uint16_t hostshort); #端口  网络转主机
        */
        #define PORT 12345
        ci->port = ntohs(client_addr.sin_port); //网络转主机
        server_addr.sin_port = htons(PORT);     //主机转网络
```

         
## 并发TCP服务器

### 多进程并发服务器
> 多进程整体结构清晰，基本无坑，但代码还有待优化
```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <wait.h>

#define IP      "0.0.0.0" 
#define PORT    12348
#define BACKLOG 128
#define INET_ADDRLENGTH 16
#define BUF_SIZE 1024

static int count = 0;
//回收子进程的信号处理函数
void handle_CHLD(int sig) {
    pid_t wpid;
    while ((wpid = waitpid(-1, NULL, WNOHANG)) > 0) {
        count--;
        printf("回收子进程：%d  剩余连接数：%d\n ", wpid, count);
        
    }
}

//服务器端程序
//测试：ip和端口写死了
int main(int argc, char  *argv[])
{
    //注册信号用于回收子进程
    struct sigaction act, oact;
    act.sa_handler = handle_CHLD;
        //设置慢速系统调用被打断后自动重启，并且只有子进程终止时才发送信号 
    act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGCHLD, &act, &oact) == -1) {
        perror("sigaction error");
        exit(1);
    }

    //创建监听socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("connect error");
        exit(1);
    }
    //常见初始化服务器监听socket地址结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);

    //给监听socket绑定地址
    int ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bine error");
        exit(1);
    }

    /*-------开始监听0.0.0.0:12348---------*/
    ret = listen(listen_fd, BACKLOG);
    if (ret == -1) {
        perror("listen error");
        exit(1);
    }

    //用于接收成功建立连接的客户端的地址结构体
    struct sockaddr_in client_addr;

    //这个用于下面accept的第3个参数，因为accept的第三个参数要求socklen_t *类型的指针，所以得先定义一个
    socklen_t cli_addr_len = sizeof(client_addr);
    
    while (1) {
        
      
        //阻塞等待建立连接
        int cfd = accept(listen_fd, (struct sockaddr *)&client_addr, &cli_addr_len);//cfd为连接成功后生成用于与客户端通信的socket
        if (cfd == -1 ) {
            perror("accept error");
            continue;
        } 
        char dst[INET_ADDRLENGTH];
        memset(dst, '\0', INET_ADDRLENGTH);
        const char *conn_ip = inet_ntop(AF_INET, &client_addr.sin_addr, dst, INET_ADDRLENGTH);
        const int conn_port = ntohs(client_addr.sin_port);
    
           //建立连接成功，创建子进程
        pid_t pid = fork();

        if (pid == 0) {
            close(listen_fd);
            int n;
            char buf[BUF_SIZE]; 
            while (1) {
                if ((n = read(cfd, buf, BUF_SIZE - 1)) > 0) {
                    write(STDOUT_FILENO, buf, n);
                } else if (n == -1 && errno == EWOULDBLOCK) {
                    continue;
                } else if (n == 0) { 
                    printf("%s:%d已经断开连接\n", conn_ip, conn_port);  // 修正了 printf
                    close(cfd);
                    exit(0);  // 退出状态改为 0（正常退出）
                } else {
                    perror("read error");
                    exit(1);
                }  
            }  
        } else {
            close(cfd);
            count++;  
            printf("%s, %d 已经成功连接，当前已连接%d个用户\n", conn_ip, conn_port, count);
        }
                  
    }  // while(1) 结束

    return 0;
}  // main 函数结束
```
### 多线程并发服务器
> 坑比较多，花费了好长时间，才实现了简陋版的优雅关闭服务器并通知所有连接着的客户端
- 文件描述符表是进程概念，所有线程共享同一份文件描述符表，是真正的共享，主线程创建的fd=3，其他线程也可以用fd=3，如果一个线程close(fd) 那么所有线程的3号文件描述符都无法使用，在这里和进程不一样
- `int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);`           这个函数的第4个参数，是要传给线程函数的参数，它是void* 就是什么都可以装，但是装的时候有坑。
    - 如果传的是指针，那么必要要传每个线程独一份的。
       1. 假如`while (1) {accept(...) ... char str[16] = "xxx" ... pthread_create(...,str)}`,根据测试每一次都会传入相同的地址。
       2. 由于1, 如果`T1`线程启动后执行较慢还未读取`str`，`accept`就接受了下一个连接，此时会修改str的值，然后创建`T2`线程，这个时候`T1`才开始读`str`那么，`T1`和`T2`会读到相同的值，这样各个线程就会产生竞争。
       3. 正确处理是应该每一个线程独一份，要用malloc传给每个线程单独的地址
    - 如果传的是值，比如需要传 `int a = 5；` 那么如果`pthread_create`的第4个参数传入`(void *)&a`,那么编译器会警告。正确处理为`(void *)(intptr_t)&a`。原因有以下几点：
          - 在`32位操作系统`指针变量大小`4个字节`，在`64位操作系统`指针变量`8个字节`
          - 如果是在`64位操作系统`中,`void *ptr = (void *)a `会在原来`4字节`的基础上强行补上`32个0`，此时再由 `int arg = (int)cfd`此时`arg`没有数据丢失
          - 但是如果是`int a = -5`,数据在计算机中是以补码存在，正数的补码反码和原码是一样的，而负数原码和补码却不一样，`void *ptr = (void *)a`会在原来的基础上补0，但是这样会导致负数的补码被破坏导致此时的a的值的大小已经变了，然后`int arg = (int)cfd`高位截断32位，然后a的值就是错上加错了。
          - 而(intptr_t)&a 在64位操作系统它是8个字节，在32位操作系统中是4个字节
          - 并且在64位操作系统中会在你是正数的时候补0，负数的时候补符号位1，这样可以保证值不变，(intptr_t)&a可以保证不管是64位还是32位，不管是正数还是负数他都能保证数据转换的正确性
- 当在使用信号时，加入ctrl + c 主动发送一个SIGINT信号，所有线程都会收到信号，但是只有一个线程能处理，谁能抢到就是谁处理。

 
```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <wait.h>
#include <pthread.h>
#include <stdatomic.h>


#define IP      "0.0.0.0"
#define PORT    12344
#define BACKLOG 128
#define INET_ADDRLENGTH 16
#define BUF_SIZE 1024
static int  NUM = 0;
volatile sig_atomic_t stop = 0;
static int lfd;
int i = 0;
int cfd_arr[1024];
struct client_info {
    int cfd;
    char ip[INET_ADDRLENGTH];
    int port;
};



void signal_handle(int sig) {
    stop = true;
    close(lfd);
    int n;
    for (n = 0; n < i; ++n) {
        shutdown(cfd_arr[n], SHUT_RD);
    }
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *mt_fun(void *arg) {
    char buf[BUF_SIZE];
    struct client_info *ci = (struct client_info *)arg;
    
    int count;
    
    
    while (!stop) {
        count = read(ci->cfd, buf, BUF_SIZE);
        if (count > 0) {
            write(STDOUT_FILENO, buf,count);
        } else if (count == 0) {
            char *msg = "服务器已经关闭！\n";
            write(ci -> cfd, msg, strlen(msg));
            break;
        } else {
                perror("read");
                
                break;
          
          
            
        }

    }
    
      
        pthread_mutex_lock(&mutex);
        NUM--;
        printf("%s:%d已经断开连接,当前用户数量：%d\n", ci->ip, ci->port, NUM);
        pthread_mutex_unlock(&mutex);
        close(ci->cfd);
        free(arg);
        pthread_exit(NULL);


    
    return NULL;
}


//多线程服务器
int main(int argc, char  *argv[])
{
    /*-----注册信号-----*/
    struct sigaction act;
   
    act.sa_handler = signal_handle;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);


    lfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);
    /*监听socket创建完成*/
    if (bind(lfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind error");
        exit(1);
    }    

    //启动监听
    if (listen(lfd, BACKLOG) == -1) {
        perror("listen error");
        exit(1);
    }

    
    while (!stop) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int cfd;
        //接受连接
        if ((cfd = accept(lfd, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
            if(errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }
       
        
        /*-----获取成功连接客户端的 cfd ip：port-------*/
        struct client_info *ci = malloc(sizeof(struct client_info));
        
        ci->cfd = cfd;
        inet_ntop(AF_INET, &client_addr.sin_addr, ci->ip, INET_ADDRLENGTH);
        ci->port = ntohs(client_addr.sin_port);
       
        cfd_arr[i++] = cfd;
        pthread_mutex_lock(&mutex);
        
        NUM++;
        printf("用户*%s:%d*已成功连接，目前用户数：%d\n", ci->ip, ci->port, NUM);
        pthread_mutex_unlock(&mutex);

        pthread_t tid;
        
        if (pthread_create(&tid, NULL, mt_fun, ci) != 0) {
            perror("create error");
            close(cfd);
            free(ci);
            continue;
        }

        pthread_detach(tid);
     

    }

    printf("服务器正在关闭...\n");
    close(lfd);
    while (NUM > 0) {
        sleep(1); //如果还有其他客户端存在，先等其他客户端关闭
    }
    printf("服务器已经彻底关闭！\n");
 
    return 0;
}




```
