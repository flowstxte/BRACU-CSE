#include <stdio.h>
int main()
{
    int a[5][2] = {{0, 0}, {1, 2}, {2, 4}, {3, 6}, {4, 8}}; // initialize a 2D array
    int i, j;

    for (i = 0; i < 5; i++) // output each array element's value
    {
        for (j = 0; j < 2; j++) // output each array element's value
        {
            printf("a[%d][%d]=%d\n", i, j, a[i][j]); // print the value of the element at location i,j
        }
    }
    return 0;
}