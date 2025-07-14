#线程与进程
    1，进程有独立的地址空间，有pcb；
        线程没有独立的地址空间，但有独立的pcb
    2，一个进程在创建一个线程后，本身退化成为了线程，在原本的进程地址空间里，存在两个线程
    3，使用ps -Lf <进程id> 可以查看该进程id下的线程的线程号LWP（不是线程id）
    4，线程间共享资源
                :)共享文件描述符
                :)每种信号的处理方式
                :)当前工作目录
                :)用户id和组id
                :)内存地址空间（.test/.data/.bss/heap/共享库）
    5，线程间非共享资源
                :)线程id
                :)处理器现场和栈指针（内核栈）
                :)独立的栈空间
                :)errno变量
                :)信号屏蔽字
                :)调度优先级

----------------------------线程控制源语----------------------------------------
#1 pthread_self

    pthread_t pthread_self(void);
    
        @brief 获得线程id，返回值成功0，不会失败

#2 pthread_create

      int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

        @brief 创建一个新线程

        @param thread           新创建出线程的线程id
        @param attr             线程属性
        @param start_routine    线程回调函数
        @param arg              回调函数的参数

#3 pthread_exit

    void pthread_exit(void *retval);

            @brief 退出一个线程

            @param 退出值
            
            @note  exit 退出进程
                   return 结束函数返回调用者
                   pthread_exit 结束当前线程，可以是main线程

#4 pthread_join

    int pthread_join(pthread_t thread, void **retval);

            @brief 阻塞等待线程退出， 获取线程退出状态

            @param thread   线程id
            @param retval   储存线程退出状态（传出参数）

#5 pthread_cancel

     int pthread_cancel(pthread_t thread);

        @brief 杀死线程id为thread的线程

        @retval 成功0，失败errno
      
        @note 如果thread线程没有到达取消点，那么pthread_cancel无效。
                此时我们可以在程序中手动添加一个取消点，pthread_testcancel();
                成功被此函数杀死的进程返回-1值

#6 pthread_detach

    int pthread_detach(pthread_t thread);

        @brief  将线程分离出去，分离出去的线程可以自己回收自己

        @retval 成功0 失败 errno

-----------------------------------------------------------------------------------------------
线程控制原语                            进程控制原语

pthread_create()                        fork()

pthread_self()                          gitpid()

pthread_exit()                          exit()

pthread_join()                          wait()/waitpid()

pthread_cancel()                        kill()

pthread_detach()(线程独有)
