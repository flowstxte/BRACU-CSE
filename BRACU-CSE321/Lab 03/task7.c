#include <stdio.h>
#include <ctype.h>

int main()
{
    FILE *fp;
    char filename[100];
    char ch;

    printf("Enter the filename: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }
    printf("Characters in the file:\n");
    while ((ch = fgetc(fp)) != EOF)
    {
        if (isalpha(ch))
        {
            printf("%c ", ch);
        }
    }
    printf("\n");
    fclose(fp);

    return 0;
}