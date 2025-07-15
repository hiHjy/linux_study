#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int products = 0;
void pthread_error(int ret, char *msg) {
    if (ret != 0) {
        fprintf(stderr, "%s error\n", msg);
        exit(1); 
    }
    
}




//条件变量实现生产者消费者
void *producer() {
    srand(time(NULL));
    while (1) {
        pthread_mutex_lock(&mutex);
        products++;
        printf("生产者,产品+1, 目前仓库有产品 %d件\n", products);
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
        sleep(rand() % 2);
    }
    
    return NULL;
}



void *consumer(void *arg) {

    int ret;
    char *who = (char *)arg; 
    
    while (1) {

        pthread_mutex_lock(&mutex);
        while (products == 0) { //防止虚假唤醒
            ret = pthread_cond_wait(&cond, &mutex);
            pthread_error(ret, "cond_wait");
        }
        /*---------------------------------------------------------*/
        products--;
        printf("消费者%s,产品-1, 目前还有产品 %d件\n", who, products);

        /*---------------------------------------------------------*/
        ret = pthread_mutex_unlock(&mutex);

        pthread_error(ret, "unlock");
        sleep(rand() % 4);
    }


    return NULL;
}




int main(int argc, char const *argv[])
{
    
    pthread_t p_tid, c_tid;   
    int ret;
    void *c1 = "黄纪元";
    void *c2 = "周兴梅";
    

    //创建生产者线程
    ret = pthread_create(&p_tid, NULL, producer, NULL);
    pthread_error(ret, "p pthread_create");
    //创建消费者线程
    ret = pthread_create(&c_tid, NULL, consumer, c1);
    pthread_error(ret, "c pthread_create");
    ret = pthread_create(&c_tid, NULL, consumer, c2);
    pthread_error(ret, "c pthread_create");
   

    pthread_exit(NULL);//主线程不会变成僵尸
    
    
}