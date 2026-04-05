#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *fib, N, D, *idx;

void *gen(void *arg)
{
    fib = (int *)malloc((N + 1) * sizeof(int));
    if (N >= 0)
        fib[0] = 0;
    if (N >= 1)
        fib[1] = 1;
    for (int i = 2; i <= N; i++)
    {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
    pthread_exit(NULL);
}

void *srch(void *arg)
{
    for (int i = 0; i < D; i++)
    {
        int j = idx[i];
        if (j >= 0 && j <= N)
        {
            printf("result of search #%d = %d\n", i + 1, fib[j]);
        }
        else
        {
            printf("result of search #%d = -1\n", i + 1);
        }
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t t1, t2;
    printf("Enter the term of fibonacci sequence: ");
    scanf("%d", &N);
    printf("How many numbers you are willing to search?: ");
    scanf("%d", &D);
    idx = (int *)malloc(D * sizeof(int));
    for (int i = 0; i < D; i++)
    {
        printf("Enter search %d: ", i + 1);
        scanf("%d", &idx[i]);
    }
    pthread_create(&t1, NULL, gen, NULL);
    pthread_join(t1, NULL);
    for (int i = 0; i <= N; i++)
    {
        printf("a[%d] = %d\n", i, fib[i]);
    }
    pthread_create(&t2, NULL, srch, NULL);
    pthread_join(t2, NULL);
    free(fib);
    free(idx);
    return 0;
}