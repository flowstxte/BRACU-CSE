#include <stdio.h>
#define PI 3.14159

float calDiameter(float radius)
{
    return 2 * radius;
}
float calCircumference(float radius)
{
    return 2 * PI * radius;
}
float calArea(float radius)
{
    return PI * radius * radius;
}

int main()
{
    float radius;
    printf("Enter the radius of the circle: ");
    scanf("%f", &radius);
    if (radius < 0)
    {
        printf("Invalid radius. Please enter a non-negative value.\n");
        return 1;
    }

    printf("Diameter: %.2f\n", calDiameter(radius));
    printf("Circumference: %.2f\n", calCircumference(radius));
    printf("Area: %.2f\n", calArea(radius));
    return 0;
}