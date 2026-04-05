#include <stdio.h>
int main()
{
    int n[10]; // n is an array of 10 integers
    int i, j;

    for (i = 0; i < 10; i++) // initialize elements of array n to 0
    {
        n[i] = i + 100; // set element at location i to i+100
    }

    for (j = 0; j < 10; j++) // output each array element's value
    {
        printf("Element[%d]=%d\n", j, n[j]); // print the value of the element at location j
    }

    return 0;
}