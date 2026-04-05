#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void main()
{
    int pid, status;
    pid = fork();
    if (pid == -1)
    {
        printf("fork failed");
        exit(1);
    }
    if (pid == 0)
    {
        printf("Child here\n");
    }
    else
    {
        wait(&status);
        printf("Well done kid\n");
    }
}