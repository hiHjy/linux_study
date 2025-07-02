#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define BUF_SIZE 4096
/**
 *  ssize_t read(int fd, void *buf, size_t count);
 * @brief 读fd，读取count字节，存在buf中
 * @param fd
 * @param buf 在read前创建一个buf,用来接收读到的数据
 * @param count sizeof(buf)
 * @retval  返回读到的实际字节大小
 *          count         (读到的buf大小字节数)   
 *          0           (没读到，因为到了文件末尾)  
 *          <count      (最后一部分剩余的内容，少于buf的大小)
 *          -1          set errno
 * @note    -1         并且errno = EAFIN 或 EWOULDBLOCK，说明不是失败，而是read非阻塞
 *                      在读一个网络文件或设备文件并且未读到数据
 *                                
 */             

 /**
  * ssize_t write(int fd, const void *buf, size_t count);     
  * @brief 从buf中读取count字节的内容向fd写入
  * @param  fd
  * @param  buf
  * @param  count 一般为count = read()
  * @retval success 写入的字节数
  *         error   -1 ,set errno
  * 
  * 
  */
//mycp 
//cp s  d
int main(int argc, char const *argv[])
{
    


    return 0;
}