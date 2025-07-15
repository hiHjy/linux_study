#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h> 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t blank_num, products;

int num = 0;

void sys_err(int ret, char *msg) {
    if (ret == -1) {
        perror(msg);
        exit(1);
    }
}



void *producer() {
    srand(time(NULL));
    while (1) {
        
            sem_wait(&blank_num);
            pthread_mutex_lock(&mutex);
            num++;
            printf("生产者,产品+1, 目前仓库有产品 %d件\n",num);
            pthread_mutex_unlock(&mutex);
            int ret = sem_post(&products);
            sys_err(ret, "sem_post");
            sleep(rand() % 2);
           
        
        
        
    }
    
    return NULL;
}



void *consumer(void *arg) {

    int ret;
    char *who = (char *)arg; 
    srand(time(NULL));
    while (1) {
        
            ret = sem_wait(&products);
            sys_err(ret, "sem_wait");
            /*---------------------------------------------------------*/
            pthread_mutex_lock(&mutex);
            num--;
            printf("消费者%s,产品-1, 目前还有产品 %d件\n", who, num);
            pthread_mutex_unlock(&mutex);
            sem_post(&blank_num);
            /*---------------------------------------------------------*/
   
        sleep(rand() % 5);

        
        
    }


}

int main(int argc, char const *argv[])
{
    int ret;
    pthread_t c_tid;
    char *c1 = strdup("黄纪元");
    char *c2 = strdup("周新梅");

    ret = sem_init(&blank_num, 0, 10);
    ret = sem_init(&products, 0, 0);
    sys_err(ret, "sem_init error");

    pthread_create(&c_tid, NULL, producer, NULL);
    pthread_create(&c_tid, NULL, consumer, c1);
    pthread_create(&c_tid, NULL, consumer, c2);

    pthread_exit(NULL);
    
    

    
}