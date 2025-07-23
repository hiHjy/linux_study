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
> - lisetn函数执行后，那么服务器就会监听绑定在监听socket上的ip和端口，然后服务器会阻塞在accept，等待客户端的连接
> - listen中第二个参数backlog指定的是可以同时有backlog个连接在一个队列中等待被accept接受，实测实际为backlog + 1个，成功被accept接受的连接，会被移出队列

-> acc
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
### TCP状态转换
> 主动关闭的一方，会进入TIME_WAIT状态，要等待 2MSL时间才会关闭

### 基本多线程并发服务器
> 坑比较多，花费了好长时间，才实现了简陋版的优雅关闭服务器并通知所有连接着的客户端
- 文件描述符表是进程概念，所有线程共享同一份文件描述符表，是真正的共享，主线程创建的fd=3，其他线程也可以用fd=3，如果一个线程close(fd) 那么所有线程的3号文件描述符都无法使用，在这里和进程不一样    
  
- 但是close(fd)只是在当前进程（或线程共享的 FD 表）中，把 FD 对应的表项删掉；但是不会中断阻塞，一个线程在 accept(lfd,…) 阻塞时，内核内部已经持有对那个打开文件描述的引用。因此，即便你在另一个线程里 close(lfd)，阻塞在 accept() 的那次调用依然把底层描述当成有效的监听 socket，继续等新的连接到来。
- 若要中断阻塞就用shutdown（fd， how）。shutdown() 不走 “文件描述符表” 这一层，而是直接操作那个底层的“打开文件描述”或“socket 对象”，改变它的状态标志：
    1. SHUT_RD：禁止后续的读操作；
    2. SHUT_WR：发送 FIN，禁止写操作；
    3. SHUT_RDWR：同时禁止读写。
        > 执行了shutdown(fd, SHUT_RDWR)函数会导致所有引用那个底层的“打开文件描述”或“socket 对象”的文件描述符关闭
- 文件描述符 vs. 打开文件描述
    1. open()/socket()⟶给进程一个小整数（file descriptor），这是“钥匙”；
    2. 内核为每个打开的对象（regular file、socket、pipe……）维护一份“打开文件描述”（open file description，或称 file object），记录状态、偏移、引用计数等。
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
### 高并发服务器
> epoll
```c
1. int epoll_create(int size);
  
    - @brief        用于创建epoll监听用的红黑树
      
    - @param size:  通知系统提前申请多大的空间
  
    - @retval       返回红黑树的文件描述符句柄
  
2. int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event); 

    - @brief            对epoll用于监听的红黑树的各种操作(epoll_create 函数的返回值) 
  
    - @param epfd:      epoll_create 函数的返回值 
    - @param op:        对监听红黑树进行什么操作：
                        - EPOLL_CTL_ADD     添加fd到 监听红黑树
                        - EPOLL_CTL_MOD     修改fd在 监听红黑树上的监听事件
                        - EPOLL_CTL_DEL     将一个fd 从监听红黑树上摘下（取消监听）   
    - @param fd:        被监听的文件描述符    
    - @param event：    对fd对应的的文件描述符，进行op操作的具体内容是什么：
                        - EPOLLIN / EPOLLOUT / EPOLLERR
                        - 例：对fd进行读监听EPOLLIN ？
  
    - @retval           成功 0 失败-1 errno

3. int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

    - @brief            阻塞监听红黑树上绑定的事件

    - @param epfd：     上同
    - @param events:    传入传出参数,传入一个struct epoll_event数组，函数调用结束，events装有所有就绪事件
    - @param maxevents：最大事件数
    - @param timeout：  epoll_wait阻塞等待多长时间？ 单位毫秒 1000ms = 1s
    
    - @retval           传入传出参数events数组满足事件的个数，后续可用于循环



```
> epoll ET模式 + 非阻塞socket中 何时内核给epoll_wait 发EPOLLIN或EPOLLOUT？

    - EPOLLIN
        1. 当客户端往server的socket的接收缓冲区写入数据成功后，内核给epoll_wait发EPOLLIN
   
    - EPOLLOUT
        1. 当你首次 epoll_ctl(..., EPOLLOUT) 注册关注 EPOLLOUT 时，只要发送缓冲区是“可写”的（也就是没有满），内核就会立刻报告一次 EPOLLOUT
        2. 当服务器端写满发送缓冲区后，send返回-1并且errno = EAGAIN时表示发送缓冲区满，当对端成功接受数据后会发送ack给服务器，此时服务器就会清出部分的发送缓冲区，此时内核发现发送缓冲区可写，会给epoll_wait发送EPOLLOUT

> send 返回值    
     
        0：成功发送了这么多字节。
        = 0：极少见，一般不会出现（不像 recv() 的 0 代表对端关闭）。

        < 0：

        errno == EAGAIN：发送缓冲区满了，非阻塞模式下你要等 EPOLLOUT 再发。

        errno == EINTR：被信号中断了，可以重试。

        其他错误：连接被关闭、网络问题等，直接关闭 socket。

    > epoll 总结：
        - epoll 本质上是个事件通知机制，它的作用就是：告诉你：哪个 fd 现在可以做某个操作了（读/写）！比如：EPOLLIN：你这个 fd 的接收缓冲区“现在有数据可读”，EPOLLOUT：你这个 fd 的发送缓冲区“现在有空间可写”。
  
        - 有两种触发模式:LT ET
          - LT:只要有数据就会触发（水平触发） #如不设置边缘触发，则默认水平触发
          - ET：只有当缓冲区状态发生变化时才触发，且只触发一次 （边缘触发）
        
        
        -  ET模式下读写一定要循环直到 EAGAIN,在 ET 模式下，EPOLLIN 和 EPOLLOUT 一旦触发后，必须将数据读完/写完直到返回 EAGAIN，否则不会再次触发。如果你不这么做，在缓冲区没读空或没写完的情况下，再次进 epoll_wait 是不会收到通知的。
    
        -epoll流程图

           