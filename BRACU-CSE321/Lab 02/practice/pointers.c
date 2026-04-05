#include <stdio.h>
int main()
{
    int var = 10; // actual variable declaration
    int *ip;      // pointer variable declaration

    ip = &var; // store address of var in pointer variable

    printf("Address of var variable: %p\n", &var); // print the address of var

    printf("Address stored in ip variable: %p\n", ip); // print the address stored in ip variable

    printf("Value of *ip variable: %d\n", *ip); // print the value of *ip variable

    return 0;
}