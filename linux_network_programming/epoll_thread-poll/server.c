#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "myepoll.h"
int main(int argc, char const *argv[])
{
    struct threadpool_t *pool = malloc(sizeof(struct threadpool_t));
    int g_efd = epoll_create(20);
    threadpool_init(pool ,g_efd);
    myepoll_init(g_efd, pool);
   

    return 0;
}