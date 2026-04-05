#include <stdio.h>
struct Student
{
    char name[50];
    int roll;
    float marks;
};

int main()
{
    struct Student s;
    printf("Enter Name: ");
    scanf("%s", s.name);
    printf("Enter Roll: ");
    scanf("%d", &s.roll);
    printf("Enter Marks: ");
    scanf("%f", &s.marks);

    printf("\nStudent Information:\n");
    printf("Name: %s\n", s.name);
    printf("Roll: %d\n", s.roll);
    printf("Marks: %f\n", s.marks);
    return 0;
}