#1 信号产生
        1，按键
        2，系统调用
        3，软件条件
        4，硬件异常
        5，命令产生
#2 两个重要的位图：未决信号集，信号屏蔽字  

   （1）未决信号集：当产生信号时，如果尚未处理，该信号对应的位变为 1，表示该信号处于挂起状态，直到被处理为止，再恢复为 0。
         注意：如果某信号已经在未决集中，再次产生同样的信号，不会“叠加”，大多数信号不排队（非实时信号）。

   （2）信号屏蔽字：内核在投递信号时会查看屏蔽字对应位是否为 1，如果为 1，表示该信号被屏蔽（不会立刻处理），信号会保留在未决信号集中，直到屏蔽解除后再处理。

#3 内核通过未决信号集（pen ding）是否还包含该信号，以及是否完成了用户定义的 signal handler 执行来判断信号是否已被处理。

    ✅ 信号的生命周期（以一个信号为例）

    假设一个 SIGINT 被送给进程：

    （1）信号产生时：

        内核将 SIGINT 对应的位设置到该进程的 未决信号集。

        如果 SIGINT 被屏蔽，则暂时不会处理，继续留在未决集。

    （2）信号准备处理时（未被屏蔽）：

        内核检查信号的处理方式：

            是 默认处理（如终止进程）？

            还是 用户定义的处理函数（signal handler）？

    （3）进入处理阶段：

        如果是默认处理，内核立即执行（如终止、忽略、生成 core dump 等）。

        如果是用户自定义处理函数：

            内核会暂时修改当前进程的执行流，让它跳转到 signal handler。

            同时从未决信号集中清除这个信号（即认为它正在处理）。

            handler 执行结束后，恢复原来程序的执行流程。

    （4）信号处理完毕的标志：

        对于用户处理函数，内核知道处理函数执行完成（通过返回地址和堆栈管理）后，认为处理完成。

        对于默认行为（终止/忽略等），处理动作由内核完成，处理完就清除。

#4信号集操作函数
        1，int sigemptyset(sigset_t *set);             #清空信号集
        2，int sigfillset(sigset_t *set);              #填满信号集（全部置1）
        3，int sigaddset(sigset_t *set, int signum);   #添加一个信号到信号集中
        4，int sigdelset(sigset_t *set, int signum);   #解除一个信号到信号集中
            （以上返回值成功0 失败-1，errno）
        5，int sigismember(const sigset_t *set, int signum); #查看signum是否已被信号集中激活
            （已被激活1，没被激活0）
        ---------------------------------------------------------------------------------------
        6，int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
            @brief 用来屏蔽信号，或解除屏蔽。其本质是读取或操作进程在PCB中的信号屏蔽字
            @param how: 假设当前信号屏蔽字为mask
                        （1）SIG_BLOCK:set 表示需要屏蔽的信号，相当于mask = mask | set
                        （2）SIG_UNBLOCK:set表示需要解除屏蔽的信号。相当于mask = mask & ~set
                        （3）SIG_SETMASK:set表示用于替代原有的屏蔽字。相当于mask = set
            @param set：自定义的set
            @param oldset： 原有的信号集
            @retval:0 , -1 errno
        7，int sigpending(sigset_t *set); 
            @brief 查看待处理（未决）信号集
            @retval:0 , -1 errno
#信号捕捉
    #include <signal.h>
    typedef void (*sighandler_t)(int);
    -1-------------------------------------------------------------------------
    sighandler_t signal(int signum, sighandler_t handler);   
    -2-------------------------------------------------------------------------
    int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
        (1)struct sigaction定义
            struct sigaction {
                    void     (*sa_handler)(int);
                    void     (*sa_sigaction)(int, siginfo_t *, void *);//想要信号携带数据用这个
                    sigset_t   sa_mask;    //只作用于信号捕捉函数运行期间，用sa_mask替换PCB中mask，传0
                    int        sa_flags;   //传0 表示默认屏蔽当前信号
                    void     (*sa_restorer)(void);//废弃
                };
        (2) retval
                0， -1 errno
       Note: 
            :)回调函数 void handler(int signo) {//} 其中signo表示信号编号，内核自动传的
            :)在回调函数执行期间，会使用用户自定义的mask，回调函数结束后会还原。
            :)在回调函数执行期间，进程会自动屏蔽当前信号，要设置act.sa_flags = 0
            :)阻塞的常规信号不支持排队，产生多次信号，只记录一次.（后32种信号支持排队）