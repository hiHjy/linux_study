#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
作用：利用文件，创建共享内存映射
参数：
    addr：指定映射区的首地址。通常传NULL，让系统自动分配。
    length：文件大小
    prot：共享内存映射区的读写属性。PROT_READ,PROT_WRITE,PROT_READ | PROT_WRITE
    flags:标注共享内存的共享属性。
        #1 MAP_SHARED   对映射区的影响会写回磁盘
        #2 MAP_PRIVATE  只会操作映射区的内容，不会写回磁盘，不会对源文件造成影响
    fd: 用于创建共享内存映射的那个文件的fd
    offset：默认0.表示映射文件全部。偏移位置，表示从哪里开始映射。需是4k的整数倍

返回值：
    成功：映射区的首地址
    失败：返回MAP_FAILED，设置errno

int munmap(void *addr, size_t length);
作用：释放mmap

注意事项：
    1，mmap用于映射的文件必须有实际大小，新创建的文件用lseek或ftruncate扩容
    2,当权限设置为MAP_SHARED时，mmap不能只写，mmap需要读fd，需要读权限
    3，mmap在创建出映射区，fd即可关闭，后续用地址访问内存共享映射区就行。
    4，offset若不是4k(4096)的整数倍，会报错：Invalid argument
    5,munmap用于释放的地址，必须是mmap申请返回的地址  
    6,mmap的权限必须 <= open的权限，且mmap必须有读权限，但是当设置MAP_PRIVATE时，只需要open有读权限，mmap可以读写
---------------------------------------------------------------------------------------
父子进程通信
    设置 #1 MAP_SHARED 父子进程共享映射区（常用这个）
        #2 MAP_PRIVATE 父子进程独占映射区
无血缘关系通讯
    1，两个进程打开同一个文件，创建映射区
    2，指定flags为MAP_SHARED
    3，一个进程写入另一个进程读出   