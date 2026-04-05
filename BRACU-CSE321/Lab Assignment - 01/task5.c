#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t child = fork();

    if (child == 0)
    {
        printf("Child process ID: %d\n", getpid());

        for (int i = 0; i < 3; i++)
        {
            pid_t grandchild = fork();

            if (grandchild == 0)
            {
                printf("Grand Child process ID: %d\n", getpid());
                exit(0);
            }
        }
        for (int i = 0; i < 3; i++)
        {
            wait(NULL);
        }
        exit(0);
    }
    else
    {
        printf("Parent process ID: %d\n", getpid());
        wait(NULL);
    }
    return 0;
}