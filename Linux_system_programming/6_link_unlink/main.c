#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
 *  int link(const char *oldpath, const char *newpath);
 *  int unlink(const char *pathname);(删除硬连接)
 *  相关命令：readlink读软连接的内容
 *  
 *  unlink 就是linux删除文件的机制，
 *  当硬连接计数减为0时，并且所有打开该文件的进程都关闭时，该文件才被操作系统择机删除
 *  @brief 创建硬连接
 *  @retval 0
 *          -1 error
 * 
 */

/**
 * int access(const char *pathname, int mode); 
 * @brief 测定指定文件是否存在/拥有某权限
 * @param pathname      文件路径
 * @param mode          F_OK 文件是否存在
 *                      R_OK, W_OK, X_OK 文件是否存在，若存在 是否具有R/W /X 权限   
 * 
 * @retval   0
 *          -1 errno
 * 
 * 
 */
/*--------------------------------------测试access函数---------------------*/
void access_test(const char *path)
{
    int ret = access(path, F_OK);
    if(ret == -1)  {
        perror("文件不存在");
        
        
    } else {
        int ret = access(path, X_OK);
        if(ret == -1) {
            printf("%s存在,但不具有X权限\n",path);
        } else {
            printf("%s存在,且具有X权限\n",path);
        }
        
    }

}

//实现mv改名

void mymv(const char *src_name, const char *dest_name)
{
     int ret = link(src_name, dest_name);
    if(ret == -1) {
        perror("link error");
        exit(1);
    }
    ret = unlink(src_name);
    
    if(ret == -1) {
        perror("unlink error");
        exit(1);
    } 


}
int main(int argc, char const *argv[])
{
    //mymv(argv[1], argv[2]);
    access_test("qweqweqwe");

    return 0;





















}