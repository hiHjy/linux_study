#include <stdio.h>
#include <unistd.h>

int main() {
    printf("No newline..."); // 不会立刻输出！
    sleep(2);
    printf("Now with newline\n"); // 这行才会一起输出
    return 0;
}

