#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
//mmap test
void mmap_test()
{
    int fd = open("../mmap_file", O_RDWR );
    if(fd == -1) {
        perror("open error");
        exit(1);
    }
    
  

    int length = lseek(fd, 0, SEEK_END); 
    char * m = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    while(1) {

        printf("%s\n", m);
        sleep(1);
    }
    
  
  
  
    
}

int main(int argc, char const *argv[])
{
    
    mmap_test();

    return 0;
}