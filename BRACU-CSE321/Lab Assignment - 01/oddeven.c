#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./oddeven num1 num2 ...\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        int n = atoi(argv[i]);
        printf("%d is %s\n", n, (n % 2 == 0) ? "Even" : "Odd");
    }
    return 0;
}
