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
    
 
 > epoll流程

          
1. 封装成带有回调函数的结构体，注册 accept 回调。

2. 将 lfd 加入 epoll 监听树（监听 EPOLLIN）。

3. 当有新连接时，触发 lfd 回调，accept 得到新的 cfd，设置为非阻塞。

4. 封装 cfd 为结构体，设置读回调，加入 epoll 监听树（监听 EPOLLIN）。

5. 当用户发来数据，触发 cfd 的读回调，处理数据，可能生成回应。

6. 读完后，可以选择：

    - 先摘下监听读（EPOLLIN），添加监听写（EPOLLOUT）。

    - 或者保留监听读，同时也监听写（用 EPOLLIN | EPOLLOUT）。

7. 当写回调被触发，发送数据，写完后可以：
    - 取消 EPOLLOUT，保留 EPOLLIN，继续监听读。
    - 如果短连接，干脆关闭连接。

```c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>

#define IP      "0.0.0.0"
#define PORT    12345
#define BACKLOG 128
#define BUFSIZE 1024
#define MAX_EVENTS 1024

struct myevent_s {
    int fd;
    uint32_t  events;
    void *arg;
    void (*call_back)(int fd, int events, void *arg);
    int status;
    char buf[BUFSIZE];
    int len;
    long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENTS + 1];

// 前置声明
void event_set(struct myevent_s *ev, int fd, int events, void *arg, 
               void (*call_back)(int, int, void *));
void event_add(int epfd, int events, struct myevent_s *ev);
void event_del(int epfd, struct myevent_s *ev);
void cb_accept_conn(int fd, int events, void *arg);
void cb_recv_data(int fd, int events, void *arg);
void cb_send_data(int fd, int events, void *arg);

void event_set(struct myevent_s *ev, int fd, int events, void *arg,
               void (*call_back)(int, int, void *)) {
    ev->fd = fd;
    ev->events = events;
    ev->arg = arg;
    ev->call_back = call_back;
    if (events & EPOLLIN) {
        memset(ev->buf, '\0', BUFSIZE);
        ev->len = 0;
    }
  
    ev->status = 0;
    
    ev->last_active = time(NULL);
}



void cb_send_data(int fd, int events, void *arg) {
   
    struct myevent_s *tmp = arg;
    
    //
    if ( epoll_ctl(g_efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
        perror("epoll_ctl:EPOLL_CTL_DEL ");
        return;
    }

    tmp->status = 0;
    printf("buf=%s, len = %d\n", tmp -> buf, tmp->len);   
    int len = send(fd, tmp->buf, tmp->len, 0 );
    if (len == -1 && errno == EAGAIN) {
        perror("不可写");
           
    } else if (len > 0) {
        printf("写回客户端%d字节\n", len);
          
    } else {
        perror("send error");
         
    }   

    
    printf("cfd=%d的客户端被移出了监听树\n", fd);
    event_set(tmp, fd, EPOLLIN | EPOLLET, tmp, cb_recv_data);
            //设置好了那就再次加到树上吧
    event_add(g_efd, EPOLLIN | EPOLLET, tmp);



}

void cb_recv_data(int fd, int events, void *arg) {
    struct myevent_s *tmp = arg;
    //写过一次后将监听节点移出监听树吧，改为监听写吧,得先移除麻对吧？
        
    if ( epoll_ctl(g_efd, EPOLL_CTL_DEL, fd, NULL) != 0) {
            perror("epoll_ctl:EPOLL_CTL_DEL ");
            return;
    }
    tmp->status = 0;
    printf("cfd=%d的客户端被移出了监听树\n", fd);
    //接受客户端的数据
    while (1) {
        int len = recv(fd, tmp->buf, BUFSIZE, 0);
        if (len > 0) {
            tmp->len = len;
            //如果len>0,恭喜我成功收到了客户端发来的数据
            write(STDOUT_FILENO, tmp->buf, len);
            printf("%s\n", tmp->buf);
            //写过一次后让服务器给cfd写吧设置监听cfd写，并设置回调cb_send_data
            event_set(tmp, fd, EPOLLOUT | EPOLLET, tmp, cb_send_data);
            //设置好了那就再次加到树上吧
            event_add(g_efd, EPOLLOUT | EPOLLET, tmp);

            
        } else if (len == 0) {
            printf("cfd=%d的客户端关闭了连接\n ", fd);
            break;
        } else if (errno == EAGAIN) {
            break;
        } else {
            perror("recv error");
            break;
        }

    } //while(1)
    
}


void event_add(int epfd, int events, struct myevent_s *ev) {
   struct epoll_event tmp; //要传给内核一个结构化的东西，所以用一个struct epoll_event类型的结构体
   tmp.data.ptr = ev;
   tmp.events = events; //让内核知道你要监听的是这个文件描述符的读还是写
   
   //先判断要监听的文件描述符，是不是已经在树上了
   if (ev->status == 1) {
        //该文件描述符已经被挂到树上了，修改就行了
        int ret = epoll_ctl(g_efd, EPOLL_CTL_MOD, ev->fd, &tmp) == 0;
        assert(ret != -1);

   } else {
        //该文件描述符第一次监听，好耶！赶紧挂到树上
        int ret = epoll_ctl(g_efd, EPOLL_CTL_ADD, ev->fd, &tmp) == 0;
        ev->status = 1;
        assert(ret != -1);
   }
   char *str;
   if (ev->events & (EPOLLIN )) { str = "EPOLLIN | EPOLLET";}
   else if (ev->events & (EPOLLOUT )) { str = "EPOLLOUT | EPOLLET";}
   printf("cfd = %d已经成功添加到监听树上, 监听时间类型:%s\n", ev->fd, str);
  
}

void cb_accept_conn(int fd, int events, void *arg) {
   //有新的连接，要创建新的socket才行哦
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int cfd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
    assert(cfd != -1);
    
    int flag = fcntl(cfd, F_GETFL);
    if (fcntl(cfd, F_SETFL, flag | O_NONBLOCK)) {
        perror("fcntl cfd");
        close(cfd);
        return;
    }
    
    //新的连接创建完毕了，用cfd与客户端通信，客户端的地址结构体也有啦----client_addr
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    printf("%s:%d 连接成功! cfd = %d\n ", ip, ntohs(client_addr.sin_port), cfd);

    //新的与客户端通信的socket已经创建成功啦，下一步要把它添加到监听树才行
        //1.从全局的自定义数组中找一个空位
        int i;
        for (i = 0; i < MAX_EVENTS; i++) { //用MAX_EVENTS的原因是g_events【MAX_EVENTS】已经被lfd占用了
            if (g_events[i].status == 0) break;
        }

        if (i == MAX_EVENTS) {
            perror("too many connections");
            close(fd);
            return;
        }
        //2.找到空位后，绑定cfd到自定义结构体上,要设置回调函数了，刚开始建立连接，一开始坑定是要先接受数据,所以绑定读回调
        event_set(&g_events[i], cfd, events, &g_events[i], cb_recv_data);
        //3.设置好回调后，将它挂到监听树上
        event_add(g_efd, events, &g_events[i]);

}

int main() {
    
    g_efd = epoll_create1(0);
    if (g_efd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    
    // 创建监听socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // 设置SO_REUSEADDR
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 绑定地址
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 监听
    if (listen(lfd, BACKLOG) == -1) {
        perror("listen");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    // 设置非阻塞
    int flags = fcntl(lfd, F_GETFL);
    if (flags == -1 || fcntl(lfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(lfd);
        exit(EXIT_FAILURE);
    }
    
    /*--------------------------将监听socket添加到监听树上，并设置回调函数为cb_accept_conn()-------------------------*/
    // event_set(struct myevent_s *ev, int fd, int events, void *arg,
    //            void (*call_back)(int, int, void *)) 
    event_set(&g_events[MAX_EVENTS], lfd, EPOLLIN | EPOLLET,&g_events[MAX_EVENTS], cb_accept_conn);
    event_add(g_efd, EPOLLIN | EPOLLET, &g_events[MAX_EVENTS]);

   /*---------------------------------------------------------------------------------------------------*/ 
    int checkpos = 0, i;
    //连接五秒的客户端后还不发送数据，那么就踢掉你
    while (1) {
        for (i = 0; i < 100; i++) {
            int idx = checkpos;
            checkpos = (checkpos + 1) % MAX_EVENTS;
            if (g_events[idx].status == 0) {
                continue;
            }
            int duration =  time(NULL) - g_events[idx].last_active;
            
            if (duration > 5) {
                
                send(g_events[idx].fd, "连接超时,连接时长\n", strlen("连接超时\n"), 0);
                g_events[idx].status = 0;
                epoll_ctl(g_efd, g_events[idx].fd, EPOLL_CTL_DEL, NULL);
                close(g_events[idx].fd);
                printf("cfd=%d的客户端,已连接时长%d,已经被踢出,\n", g_events[idx].fd, duration);
                
            } 

        }

        //readys数组给epoll_wait用，用于传出处于监听树上的节点有哪些节点响应，nfds为响应节点的数量，后面用于处理响应
        struct epoll_event readys[MAX_EVENTS + 1];
        int nfds = epoll_wait(g_efd, readys, MAX_EVENTS + 1, 1000);
        
        
        //处理响应
        int i;
        for (i = 0; i < nfds; i++) {
            struct myevent_s *tmp = readys[i].data.ptr; //用于获取节点的自定义结构体
            if ((readys[i].events & (EPOLLIN | EPOLLET)) /*节点实际响应的事件*/ && (tmp->events & (EPOLLIN | EPOLLET))/*用户想要响应的事件*/ ) {
                tmp->call_back(tmp->fd, tmp->events, tmp->arg);
            } 

            if ((readys[i].events & (EPOLLOUT | EPOLLET)) /*节点实际响应的事件*/ && (tmp->events & (EPOLLOUT | EPOLLET))/*用户想要响应的事件*/ ) {
                tmp->call_back(tmp->fd, tmp->events, tmp->arg);
            } 


        }
        
        
       
    }//while(1)

    
    close(g_efd);
    return 0;
}



```