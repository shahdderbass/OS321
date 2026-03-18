/*
 * Shahd Derbass
 * 2231172085
 * Lab06
 */

#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("User ID: %d\n", getuid());
    printf("Group ID: %d\n", getgid());
    return 0;
}
