#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
    int fd;
    char buffer[80];
    static char message[] = "Hello, World!\n";
    fd = open("myfile", O_RDWR | O_CREAT, 0666);
    if (fd != -1)
    {
        printf("myfile opened for read/write access\n");
        write(fd, message, sizeof(message));
        lseek(fd, 0, 0);
        read(fd, buffer, sizeof(message));
        printf(" %s was written to myfile\n", buffer);
        close(fd);
    }
    return 0;
}