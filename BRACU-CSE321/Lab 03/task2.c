#include <stdio.h>

int main()
{
    int angle1, angle2, angle3;
    printf("Enter the three angles of the triangle: ");
    printf("Angle 1: ");
    scanf("%d", &angle1);
    printf("Angle 2: ");
    scanf("%d", &angle2);
    printf("Angle 3: ");
    scanf("%d", &angle3);
    if (angle1 > 0 && angle2 > 0 && angle3 > 0 && angle1 + angle2 + angle3 == 180)
    {
        printf("The triangle is valid.\n");
    }
    else
    {
        printf("The triangle is invalid.\n");
    }
    return 0;
}