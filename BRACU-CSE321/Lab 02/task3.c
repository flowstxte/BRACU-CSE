#include <stdio.h>
#define PI 3.1416
int main()
{
    float r;
    printf("Enter radius: ");
    scanf("%f", &r);

    float area = PI * r * r;
    float perimeter = 2 * PI * r;

    printf("Area of the circle: %f\n", area);
    printf("Perimeter of the circle: %f\n", perimeter);

    return 0;
}