#include <stdio.h>

int main()
{
    int n, sum = 0, i;
    printf("Enter a positive integer: ");
    scanf("%d", &n);

    if (n <= 0)
    {
        printf("Invalid input. Please enter a positive integer.\n");
        return 1;
    }

    for (i = 1; i <= n; i++)
    {
        if (i % 2 == 0)
        {
            sum += i;
        }
    }

    printf("The sum of even numbers from 1 to %d is %d\n", n, sum);

    return 0;
}