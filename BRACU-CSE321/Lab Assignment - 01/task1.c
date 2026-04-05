#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char input[256];

    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "a");
    if (fp == NULL)
    {
        printf("Error opening or creating file\n");
        return 1;
    }

    printf("File %s opened successfully.\n", argv[1]);
    printf("Enter '-1' to exit.\n");

    while (1)
    {
        printf("Enter string: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Error reading input or limit reached\n");
            break;
        }
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "-1") == 0)
        {
            printf("Exiting program\n");
            break;
        }
        fprintf(fp, "%s\n", input);
    }
    fclose(fp);
    printf("File %s closed successfully\n", argv[1]);
    return 0;
}