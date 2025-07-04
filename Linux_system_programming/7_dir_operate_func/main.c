#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
/**
 *  DIR *opendir(const char *name);
 *  struct dirent *readdir(DIR *dirp);
 *  int closedir(DIR *dirp);
 *  @brief 目录操作函数(重点readdir)
 *  @retval 
 *          opendir:
 *                  success 得到一个文件流，类似于fd
 *                  error   NULL errno
 *          readdir：
 *                  success 1，得到目录项结构体指针，类似于open一个文件后的读写指针，目录的内容就是目录项
 *                          2，NULL 读到末尾末尾
 * 
 *                  error   NULL && errno 错误
 * 
 * @note <dirent.h>
 */     



/*-------------------------------目录项结构体----------------------------------------*/
    //  struct dirent {
    //            ino_t          d_ino;       /* Inode number */
    //            off_t          d_off;       /* Not an offset; see below */
    //            unsigned short d_reclen;    /* Length of this record */
    //            unsigned char  d_type;      /* Type of file; not supported
    //                                           by all filesystem types */
    //            char           d_name[256]; /* Null-terminated filename */
    //        };
/*--------------------------------实现ls----------------------------------------------*/
int main(int argc, char const *argv[])
{
    DIR *fs = opendir(argv[1]);
    
    struct dirent *ret;
    if(fs == NULL) {
        perror("opendir error");
        exit(1);
    }
    while((ret = readdir(fs))) {
       printf("%s  ", ret->d_name);
        
    }
    putchar('\n');
    closedir(fs);
    


    return 0;
}