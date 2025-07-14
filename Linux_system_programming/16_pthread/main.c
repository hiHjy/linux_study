#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
struct rtv {
    int val;
    char msg[256];

};
//测试pthread_cancel，必须进内核，或设置取消点pthread_cancel();
// void* start_routine(void * arg) {
   

//     while(1) {
//         if(1) {

//         } else {

//         }
//     }   
//     return (void *)11;
// }

void* start_routine(void * arg) {

    int no = *(int *) arg;
    printf("----thread---进程id:%d, 第%d个线程,线程id%lu\n",
         getpid(), no, pthread_self());
    
    return (void *)11;
}
//循环创建五个子线程
pthread_t *pthread_test() {
    pthread_t tid;
    int i, ret, real_arg[5];
    pthread_t *threads = malloc(sizeof(pthread_t) * 5);
    
    printf("----main---进程id:%d, 线程id%lu\n", getpid(),pthread_self());  
    for (i = 0; i < 5; ++i) {
        real_arg[i] = i + 1;
        ret = pthread_create(&tid,
         NULL, start_routine, &real_arg[i]);
        threads[i] = tid;
        if (ret != 0) {
            perror("pthread_create error");
            exit(1);
        } 
        
    }
    sleep(3);
  return threads;
            
}

//用pthread_join回收循环创建的5个子进程
void pthread_join_test(pthread_t threads[]) {

    int ret, i;
    void* retval;
    for (i = 0; i < 5; ++i) {
        ret = pthread_join(threads[i], &retval);//*retval
        if (ret != 0) {
            perror("pthread_join error");
            exit(1);
        }
        printf("线程：%lu已经成功回收，返回值%d\n", threads[i], (int)(intptr_t)retval);

    }
    



}
   
//pthread_cancel测试
void pthread_cancel_test() {
    void *rtv;
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, start_routine, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_create error:%s\n", strerror(ret));
        exit(1);
    }

    ret = pthread_cancel(tid);
    if (ret != 0) {
        fprintf(stderr, "pthread_cancel error:%s\n", strerror(ret));
        exit(1);
    }
    pthread_join(tid, &rtv);
     if (ret != 0) {
        fprintf(stderr, "pthread_join error:%s\n", strerror(ret));
        exit(1);
    }
    printf("被杀死线程为：%lu, 退出值为%d\n", tid, (int)(intptr_t)rtv);

}
//测试pthread_detach
void pthread_detach_test() {
    void *rtv = (void *)1;
    
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, start_routine, &rtv);
    if (ret != 0) {
        fprintf(stderr, "pthread_create error:%s\n", strerror(ret));
        exit(1);
    }
    ret = pthread_detach(tid);
    if (ret != 0) {
        fprintf(stderr, "pthread_cancel error:%s\n", strerror(ret));
        exit(1);
    }
    sleep(3);
    ret = pthread_join(tid, &rtv);
     if (ret != 0) {
        fprintf(stderr, "pthread_join error:%s\n", strerror(ret));
        exit(1);
    }

}

int main(int argc, char const *argv[])
{
    //pthread_join_test(pthread_test());
    //pthread_cancel_test();
    pthread_detach_test();

    return 0;
}