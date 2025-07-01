#include <stdio.h>
#include "../inc/add.h"
int main(int argc, char const *argv[])
{
    int a = 14, b = 17;
    printf("hello world\n");
    
     printf("%d+%d=%d\n", a, b, add(a, b));
    return 0;
}
