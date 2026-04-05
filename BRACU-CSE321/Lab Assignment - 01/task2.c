#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t c = fork();

    if (c == 0)
    {
        pid_t c2 = fork();
        if (c2 == 0)
        {
            printf("I am grandchild\n");
        }
        else
        {
            wait(NULL);
            printf("I am child\n");
        }
    }
    else
    {
        wait(NULL);
        printf("I am parent\n");
    }

    return 0;
}