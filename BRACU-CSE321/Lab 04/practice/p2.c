#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
    printf("Program-2 Running...");
    pid_t pid, status;
    pid = fork();
    if (pid == 0)
    {
        execl("/home/flowstxte/Wall/Lab 04/practice/p1", "p1", "a", "b", "c", "d", NULL);
    }
    else if (pid > 0)
    {
        wait(&status);
        execl("/bin/pwd", "pwd", NULL);
        perror("execl failed");
    }
    return 0;
}