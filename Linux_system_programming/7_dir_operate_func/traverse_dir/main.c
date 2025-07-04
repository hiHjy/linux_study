#define _DEFAULT_SOURCE
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>




/*--------------------------------递归遍历目录----------------------------------------------*/



void traverse_dir(const char *path)
{
    struct stat statbuf;
    char *tmp;
    int ret = stat(path, &statbuf); 
    if(ret == -1) {
        perror("stat error");
        exit(1);
    }

    int st_mode = statbuf.st_mode;
    if(S_ISDIR(st_mode)) {
        //如果是目录
        
        DIR *dp = opendir(path);
        if(dp == NULL) {
            perror("opendir error");
        }
        struct dirent *sd;
        while((sd = readdir(dp))) {
             if(strcmp(sd->d_name, ".") ==0 || strcmp(sd->d_name, "..") == 0) {
                continue;         
            }
            //拼路径
            int len = strlen(path) + strlen(sd->d_name) + 2;
            tmp = malloc(len);
            strcpy(tmp, path);
            strcat(tmp, "/");
            strcat(tmp, sd->d_name);
           
            //如果该目录项是目录就递归调用
            if(sd->d_type == DT_DIR) {
            
                    traverse_dir(tmp);
                    free(tmp); 
                   
            } else {
                //否则输出文件内容
                int ret = write(STDOUT_FILENO, tmp, strlen(tmp));
                putchar('\n');
                if(ret == -1) {
                    perror("write error");
                    exit(1);
                }
            }
        }
        
        closedir(dp);
    }

    







}
int main(int argc, char const *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "usage <./main> <dir>\n");
        exit(1);
    }
    traverse_dir(argv[1]);


    return 0;
}