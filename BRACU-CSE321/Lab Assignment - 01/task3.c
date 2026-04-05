#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main()
{
    pid_t a, b, c;
    pid_t ipid = getpid();

    int *cnt = mmap(0, sizeof(int), 3, 0x21, -1, 0);
    *cnt = 1;

    printf("Parent process (PID: %d)\n", ipid);

    a = fork();
    if (a > 0)
        (*cnt)++;

    b = fork();
    if (b > 0)
        (*cnt)++;

    c = fork();
    if (c > 0)
        (*cnt)++;

    if (getpid() % 2 == 1)
    {
        if (fork() > 0)
        {
            (*cnt)++;
        }
    }

    printf("Process PID: %d\n", getpid());

    if (getpid() == ipid)
    {
        sleep(1);
        printf("\nTotal processes created: %d\n", *cnt);
        while (wait(NULL) > 0)
            ;
        munmap(cnt, sizeof(int));
    }
    return 0;
}