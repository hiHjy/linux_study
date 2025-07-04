#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
 #include <sys/stat.h>
/*创建一个文件并且默认可执行*/
/**
 * @brief man 7 inode  
 * 
 * 
 *         S_IRWXU     00700   owner has read, write, and execute permission
           S_IRUSR     00400   owner has read permission
           S_IWUSR     00200   owner has write permission
           S_IXUSR     00100   owner has execute permission

           S_IRWXG     00070   group has read, write, and execute permission
           S_IRGRP     00040   group has read permission
           S_IWGRP     00020   group has write permission
           S_IXGRP     00010   group has execute permission

           S_IRWXO     00007   others  (not  in group) have read, write, and
                               execute permission
           S_IROTH     00004   others have read permission
           S_IWOTH     00002   others have write permission
           S_IXOTH     00001   others have execute permission
 * 
 * 
 * 
 * 
 * 
 */
/*--------------------创建一个文件，默认有x权限--------------------------------*/
int main(int argc, char const *argv[])
{
    
    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    close(fd);

    struct stat sb;

    int ret = stat(argv[1], &sb);
    if(ret == -1) {
        perror("stat error");
        exit(1);
    }    

    int mode = sb.st_mode & 07777;
    chmod(argv[1], mode | S_IXUSR);

    return 0;
}