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
 *          count        (读到的buf大小字节数)   
 *          0           (没读到，因为到了文件末尾)  
 *          <count      (最后一部分剩余的内容，少于buf的大小)
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
    if(argc < 3){
      fprintf(stderr, "Usage:%s <src_file> <dest_dir/>\n", argv[0]);
      return -1;
    }
  
    char buf[BUF_SIZE];
   

    /*---------------拼出目标路径---------------------------------*/
    int target_path_len = strlen(argv[1]) + strlen(argv[2]) + 1;
    char* target_path = malloc(target_path_len);
    if(target_path == NULL) 
      printf("malloc error\n");
      
    strcpy(target_path, argv[2]);
    strcat(target_path, argv[1]);
    
    int src_fd = open(argv[1], O_RDONLY);
    if(src_fd == -1){
      printf("%s\n",strerror(errno));
      return -1;
    }
    
    int target_fd = open(target_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(src_fd == -1){
      printf("%s\n",strerror(errno));
      return -1;

    }
    /*---------------开始复制---------------------------------*/
    int read_Byte = read(src_fd, &buf, sizeof(buf));
    while (read_Byte > 0)
    {
        write(target_fd, buf, read_Byte);
        read_Byte = read(src_fd, &buf, sizeof(buf));
    }
    close(src_fd);
    close(target_fd);
    free(target_path);
    



    return 0;
}