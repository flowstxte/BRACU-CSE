#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        printf("Child process\n");
    }
    else if (pid > 0)
    {
        printf("Parent process. Child process ID is %d\n", pid);
    }
    else
    {
        perror("fork failed");
    }
    return 0;
}