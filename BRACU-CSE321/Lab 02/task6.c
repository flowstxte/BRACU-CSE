#include <stdio.h>
int main()
{
    int seconds, h, m, s;
    printf("Enter time in seconds: ");
    scanf("%d", &seconds);

    h = (seconds / 3600);
    seconds %= 3600;
    m = (seconds / 60);
    s = (seconds % 60);

    printf("H:M:S - %d:%d:%d\n", h, m, s);

    return 0;
}