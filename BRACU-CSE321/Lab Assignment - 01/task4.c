#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./task4 num1 num2 ...\n");
        return 1;
    }
    char **args = malloc((argc + 1) * sizeof(char *));
    for (int i = 1; i < argc; i++)
        args[i] = argv[i];
    args[argc] = NULL;

    pid_t p = fork();

    if (p == 0)
    {
        args[0] = "./sort";
        execv("./sort", args);
    }
    else
    {
        wait(NULL);
        args[0] = "./oddeven";
        execv("./oddeven", args);
    }
    return 0;
}