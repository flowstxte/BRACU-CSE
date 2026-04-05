#include <stdio.h>

int main()
{
    FILE *fp;
    char buff[100];

    fp = fopen("file2.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return -1;
    }

    fscanf(fp, "%s", buff);
    printf("1 : %s\n", buff);

    fgets(buff, sizeof(buff), (FILE *)fp);
    printf("2: %s\n", buff);

    fgets(buff, sizeof(buff), (FILE *)fp);
    printf("3: %s\n", buff);
    fclose(fp);
}