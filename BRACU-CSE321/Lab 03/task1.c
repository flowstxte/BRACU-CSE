#include <stdio.h>
#include <ctype.h>

int main()
{
    char ch;
    printf("Enter a character: ");
    scanf("%c", &ch);
    ch = tolower(ch);

    if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
    {
        printf("The character is a vowel.\n");
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        printf("The character is a consonant.\n");
    }
    else
    {
        printf("Invalid input. Please enter a valid character.\n");
    }
    return 0;
}