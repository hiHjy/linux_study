#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *  int link(const char *oldpath, const char *newpath);
 *  int unlink(const char *pathname);(删除硬连接)
 *  unlink 就是linux删除文件的机制，
 *  当硬连接计数减为0时，并且所有打开该文件的进进程都关闭时，该文件才被操作系统择机删除
 *  @brief 创建硬连接
 *  @retval 0
 *          -1 error
 * 
 * 
 */
//实现mv改名
int main(int argc, char const *argv[])
{
    int ret = link(argv[1], argv[2]);
    if(ret == -1) {
        perror("link error");
        exit(1);
    }
    ret = unlink(argv[1]);
    
    if(ret == -1) {
        perror("link error");
        exit(1);
    }
    return 0;
}