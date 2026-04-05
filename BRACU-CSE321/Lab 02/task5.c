#include <stdio.h>
int main()
{
    int n;
    printf("Enter number of values: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter %d values:\n", n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }

    printf("Reverse order:\n");
    for (int i = n - 1; i >= 0; i--)
    {
        printf("%d ", arr[i]);
    }

    return 0;
}