#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#define MAX_TASK_NUM 100
#define MIN_TASK_NUM 20
#define TASK_QUEUE_MAX_SIZE 10
#define THREAD_ADJUST_SIZE 20
struct threadpool_task_t {
    void (*functon)(int, int, void *);
    int fd;
    int events;
    void *arg;
};

struct threadpool_t {
    pthread_mutex_t lock;           
    pthread_mutex_t lock_thread_count;          //处于忙状态的计数锁
    pthread_cond_t queue_not_empty; 
    pthread_cond_t queue_not_full;
    int shutdown;                           //线程池开始工作false 不工作true

    struct threadpool_task_t *task_queue;   //任务队列
    int task_count;;         //           //任务队列中当前任务数量
    int task_queue_max_size;                //任务队列最大任务数
    int busy_thr_num;   //
    int max_thr_num;                        
    int min_thr_num;                        
    int queue_front;    //
    int queue_rear;     //
    int live_thr_num;   //0    
    int wait_exit_thr_num;//                  //要杀死的进程数量
   
    pthread_t adjust_tid;                   //管理线程的线程id
    pthread_t *thr_tids;                  //所有线程的线程id数组
    int epfd;

};
void threadpool_init(struct threadpool_t *pool, int);
void task_add(struct threadpool_t *pool,void (*function)(int, int, void *),int fd, int events, void *arg);
void task_read (int fd, int events, void *arg);
void threadpool_destroy(struct threadpool_t *pool);
void task_write(int fd, int events, void *arg);
#endif