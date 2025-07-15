一，互斥锁
    互斥锁是建议锁，所有线程应该在访问公共数据前先拿锁再访问，但锁本身不具有强制性，也就是说线程如果不使用锁是 可以直接访问公共数据的。

    #1 pthread_mutex_t lock;    创建锁

    #2 pthread_mutex_init;      初始化锁 

    #3 pthread_mutex_lock;      加锁 --mutex 阻塞线程

    #4 访问共享数据

    #5 pthread_mutex_unlock     解锁 ++mutex 唤醒阻塞在锁上的线程

    #6 pthread_mutex_destroy    销毁锁


    注意： #1 trylock加锁失败直接返回errno，不阻塞
          #2 初始化mutex也可以这样 pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

二，读写锁
    1，读写锁是“写模式加锁”时，解锁前，所有对该锁加锁的线程都会被阻塞
    2，读写锁是“读模式加锁”时，如果线程以读模式对其加锁会成功；如果线程以写模式加锁会阻塞
    3，读写锁是“写模式加锁”时，阻塞要写的和要读的线程，那么会优先满足要写的线程。一句话说就是：
        读锁，写锁并行阻塞，写锁优先级高
    # 读写锁也叫共享-独占锁。当读写锁以读模式锁住时，他是以共享模式锁住的；当他以写模式模式锁住时，它是以独占模式锁住的。写独占，读共享。
    # 读写锁适合对数据结构读的次数远大于写的情况

    pthread_rwlock_init

    pthread_rwlock_destroy

    pthread_rwlock_rdlock

    pthread_rwlock_wrlock

    pthread_rwlock_tryrdlock

    pthread_rwlock_trywrlock

    pthread_rwlock_unlock

    返回值都是：成功 0 ，失败返回errno

    pthread_rwlock_t 读写锁类型

三，死锁
    1，反复加锁
    2，t1拿着a锁请求b锁，t2拿着b锁请求a锁

四，条件变量
    #1 条件变量本身不是锁，但它可以造成线程阻塞。通常与互斥锁结合使用。给多线程提供一个会合的场所。

    pthread_cond_init

    pthread_cond_destroy

    pthread_cond_wait   等待一个条件满足

        #1 函数原型  int pthread_cond_wait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex);

        #2 函数作用 pthread_cond_wait() 会：

                1，自动释放 mutex 锁，并将线程挂起。

                2，等待条件变量 cond 被 pthread_cond_signal() 或 broadcast() 唤醒。

                3，被唤醒后自动重新加锁 mutex，继续执行后续逻辑。

                4，有可能虚假唤醒：
                    虚假唤醒就是指：一个线程调用 pthread_cond_wait() 等待条件变量时，虽然没有任何线程调用 pthread_cond_signal() 或 broadcast()，它却“莫名其妙被唤醒”了。简单说：你没有被叫醒，但你自己醒了。所以在调用这个函数放在一个while（条件）里


                   
    pthread_cond_signal 通知阻塞的线程

    pthread_cond_broadcast  通知所有堵塞的线程

    以上6个函数返回值：成功0 失败errno

    pthread_cond_t 类型 用于定义条件变量

   #2 初始化条件变量

        第一种   pthread_cond_t cond;
                pthread_cond_init(&cond, NULL);

        第二种   pthread_cond_t cond = PTHREAD_COND_INITIALIZER;静态初始化

                
三，信号量
    相当于初始化值为N的互斥量。N值表示可以同时访问共享数据区的线程数。
    
#1 函数

        int sem_init(sem_t *sem, int pshared, unsigned int value);   

            @param pshared  0 ：线程同步 
                            1 ：用于进程同步
            @param value    N值 指定可以同时访问的线程/进程数

        sem_destroy

        sem_wait    //相当于加锁sem--,如果信号量sem--后sem<0，则阻塞

        sem_trywait

        sem_timedwait

        sem_post    //相当于解锁sem++，同时唤醒阻塞在信号量上的线程/进程

        @note 

            # 以上6个函数成功0失败-1，同时设置errno
            # sem_t 类型，本质是结构体，使用期间看做整数，忽略实现细节（类似使用文件描述符）
            # sem_t sem 规定信号量sem不能小于0。头文件<semaphore.h>
            # 由于sem_t的实现对用户隐藏，所以所谓的++、--操作只能通过函数实现
            # 信号量的初值，决定了占用信号量线程/进程的个数