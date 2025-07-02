#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/**
 *  off_t lseek(int fd, off_t offset, int whence);
 *  @brief  移动文件读写位置指针
 *  @param  fd
 *  @param  offset  偏移量
 *  @param  whence  起始偏移位置
 *                  SEEK_SET(开头) 
 *                  SEEK_CUR(当前位置) 
 *                  SEEK_END(结尾)
 *  @retval 成功：较起始位置的偏移量
 *          失败：-1 errno
 *  @note     作用1：lseek(fd, 0, SEEK_SET); 读写位置指针置0
 *            作用2：file_size = lseek(fd, 0, SEEK_END); 返回文件大小
 *            作用3：lseek(fd, size, SEEK_END) ; 拓展文件大小(扩大size字节)
 */
int main(int argc, char const *argv[])
{
    /*----------------读写位置指针置0------------------------------------*/
    char str[] = "it is lseek test\n";
    int fd = open("./lseek_test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    char buf,count;
    //计算可见字符用strlen（）,写入./lseek_test.txt后读写指针到达文件末尾
    write(fd, str, strlen(str)); 
    //读写位置指针置0，便于后续操作
    lseek(fd, 0, SEEK_SET);
    sleep(2);
    while((count = read(fd, &buf, 1)) > 0) {
        write(STDOUT_FILENO, &buf, count);
    }
    close(fd);
    /*----------------------------算文件大小(野路子)------------------------------*/
    int fd1 = open(argv[1], O_RDWR);
    if(fd1 == -1) {
        perror("open error argv[1]");
        exit(1);
    }
    printf("%s size:%ld\n", argv[1], lseek(fd1, 0, SEEK_END));
    
    
/*----------------------------拓展文件大小(野路子)(必须要有io操作）------------------------------*/

    //拓展1000字节
    long int size = lseek(fd1, 999, SEEK_END);
    //引起io 999 + 1 = 1000
    int write_count =  write(fd1, "a", 1);
    if(write_count == -1) {
        perror("write 1 Byte error");
    }
    printf("%s size:%ld\n", argv[1], size + 1);
    close(fd1);


/*------------------拓展文件大小(正规军）-----------------------------------*/
 /**
  *   int truncate(const char *path, off_t length);
  * @brief 指定文件的大小 
  * @param path 只读文件路径，文件必须存在
  * @param length 指定文件大小
  * @retval success 0
  *         error   -1 errno
  * @note 文件必须是可写的
  *       length < file_size 数据会丢失
  *       length > file_size 多的内容实测会乱码
  */       
  
    truncate("./lseek_test.txt", 2000);
    
    return 0;
}