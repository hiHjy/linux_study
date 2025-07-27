#include "thread_pool.h"
#include "myepoll.h"
//线程池任务结构体
int g_efd;




//线程工作函数
void *thread_work(void *arg) {
    struct threadpool_t *pool = arg;
    
    while(!pool->shutdown) {
        pthread_mutex_lock(&pool->lock);
        
        while (pool->task_count == 0 && pool->shutdown == false) {
            if (pool->wait_exit_thr_num > 0) {
                pthread_mutex_lock(&pool->lock);
                pool->wait_exit_thr_num--;
                pool->live_thr_num--;
                pthread_mutex_unlock(&pool->lock);
                //空闲进程自杀
                printf("空闲线程%lu已自杀\n", pthread_self());
                pthread_exit(NULL);
            }
            pthread_cond_wait(&pool->queue_not_empty, &pool->lock);
        }
        //程序到这里说明已经拿着锁，并且有队列中任务，直接干一票
            //取出下标尽快释放锁
        int index = pool->queue_front;;
        pool->queue_front = (pool->queue_front + 1) % TASK_QUEUE_MAX_SIZE;
        pool->task_count--;
        pthread_mutex_unlock(&pool->lock);
        pthread_cond_signal(&pool->queue_not_full);

        pthread_mutex_lock(&pool->lock_thread_count);               //此时算这个线程是繁忙的
        pool->busy_thr_num++;
        pthread_mutex_unlock(&pool->lock_thread_count); 
        // struct threadpool_task_t {
        //     void (*functon)(int, int, void *);
        //     int fd;
        //     int events;
        //     void *arg;
        // };

        pool->task_queue[index].functon(pool->task_queue[index].fd, pool->task_queue[index].events,pool->task_queue[index].arg);   //主要任务
        
        pthread_mutex_lock(&pool->lock_thread_count);
        pool->busy_thr_num--;
        pthread_mutex_unlock(&pool->lock_thread_count);
       
     
        
    }
    pthread_exit(NULL);

} 

//调整线程函数
void *thread_adjust(void *arg) {
    
    struct threadpool_t *pool = arg; 
    while (!pool->shutdown) {
        sleep(10);
        pthread_mutex_lock(&pool->lock_thread_count);
        //获取负载
        int busy = pool->busy_thr_num;
        int live = pool->live_thr_num;
        pthread_mutex_unlock(&pool->lock_thread_count);
        
    
        if (busy * 2 > live && live < pool->max_thr_num && live < MAX_TASK_NUM) {
            int i;
            pthread_mutex_lock(&pool->lock);
            for (i = 0; i < THREAD_ADJUST_SIZE; i++) {
                pool->live_thr_num++;
                if (pool->live_thr_num >= MAX_TASK_NUM) {
                   fprintf(stderr, "线程数超限\n"); 
                   break;
                }
                if (pthread_create(&pool->thr_tids[pool->live_thr_num], NULL, thread_work, pool)) {
                    
                    fprintf(stderr, "扩容时创建线程失败\n");
                    pool->live_thr_num--;
                    break;
                }
                pthread_detach(pool->thr_tids[pool->live_thr_num]);
            }
            pthread_mutex_unlock(&pool->lock);
        }

        if (busy * 4 < live && live > pool->min_thr_num && live > MIN_TASK_NUM) {
            
            pthread_mutex_lock(&pool->lock);
            pool->wait_exit_thr_num = THREAD_ADJUST_SIZE;
            pthread_mutex_unlock(&pool->lock);
            pthread_cond_broadcast(&pool->queue_not_empty);
        }
        if (pool->shutdown) {
            break;
        }
    }
    pthread_exit(NULL);
}

void task_processing(void *arg) {
     sleep(2);
    printf("%lu干活了,参数为%d\n", pthread_self(), (int)(intptr_t)arg);
   
}

void threadpool_init(struct threadpool_t *pool, int epfd) {
    g_efd = epfd;
    //创建线程池
    pool->shutdown = false;

    pool->max_thr_num = MAX_TASK_NUM;
    pool->min_thr_num = MIN_TASK_NUM;
    pool->task_queue_max_size = TASK_QUEUE_MAX_SIZE;
    pool->task_queue = malloc(sizeof(struct threadpool_task_t) * TASK_QUEUE_MAX_SIZE);
    pool->thr_tids = malloc(sizeof(pthread_t) * MAX_TASK_NUM);

    pool->busy_thr_num = 0;
    pool->live_thr_num = 0;
    pool->wait_exit_thr_num = 0;
    pool->queue_front = 0;
    pool->queue_rear = 0;
    pool->task_count = 0;
  

    if (pthread_mutex_init(&(pool->lock), NULL) 
        || pthread_mutex_init(&(pool->lock_thread_count), NULL)
        || pthread_cond_init(&(pool->queue_not_empty), NULL) 
        || pthread_cond_init(&(pool->queue_not_full), NULL)) {
        
        perror("threadpool init error");
        exit(1);
    }
    
    //先创建好最初MIN_TASK_NUM的线程
    int i;
    for (i = 0; i < MIN_TASK_NUM; ++i) {
        if (pthread_create(&pool->thr_tids[i], NULL, thread_work, pool) != 0) {
            perror("work_pthread create error");
            exit(1);
        }
        pthread_detach(pool->thr_tids[i]);
        pool->live_thr_num++;
    }

    if (pthread_create(&pool->adjust_tid, NULL, thread_adjust, pool) != 0) {
        perror("adjust_pthread create error");
    }

    pthread_detach(pool->adjust_tid);
 
    //printf("%d\n", pool->live_thr_num);

}
void task_write(int fd, int events, void *arg) {

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
void task_read (int fd, int events, void *arg) {
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
// void task_add(struct threadpool_t *pool,void (*function)(int, int, void *),
//  int fd, int events, void *arg);
void task_add(struct threadpool_t *pool, void (*function)(int, int, void*) ,int fd, int events, void *arg) {
    pthread_mutex_lock(&pool->lock);
   

    while (pool->task_count == TASK_QUEUE_MAX_SIZE) {
        //防止虚假唤醒，说明队满，得堵塞在这里等signal或者board的什么
        pthread_cond_wait(&pool->queue_not_full, &pool->lock);

    }

    //说明队列不满，可以添加任务
    // struct threadpool_task_t {
    //     void (*functon)(int, int, void *);
    //     int fd;
    //     int events;
    //     void *arg;
    // };

    pool->task_queue[pool->queue_rear].functon = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->task_queue[pool->queue_rear].fd = fd;
    pool->task_queue[pool->queue_rear].events = events;
    
    pool->queue_rear = (pool->queue_rear + 1) % TASK_QUEUE_MAX_SIZE;
    pool->task_count++;
    pthread_mutex_unlock(&pool->lock);
    //任务添加好了，通知一部分线程可以开始干活了
    pthread_cond_signal(&pool->queue_not_empty);

}


void threadpool_destroy(struct threadpool_t *pool) {
    //回收内存
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->queue_not_empty); //空闲的进程立刻退出
    pthread_mutex_lock(&pool->lock); //等待正在工作的线程结束
    int i = 10;
    while (pool->live_thr_num > 0) {
        
        printf("正在等待工作中的进程退出,倒计时:%d\n", i);
        sleep(1);
        if (--i == 0) {
            break;
        }
    }
    
    pthread_mutex_destroy(&pool->lock);
    pthread_mutex_destroy(&pool->lock_thread_count);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_cond_destroy(&pool->queue_not_full);


    free(pool->thr_tids);
    free(pool->task_queue);
    free(pool);
   
}

