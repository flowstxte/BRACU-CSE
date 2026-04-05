#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

struct shared
{
    char sel[100];
    int b;
};
static void trim(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[n - 1] = '\0';
}

int main(void) {
    int id = shmget(IPC_PRIVATE, sizeof(struct shared), IPC_CREAT | 0666);
    struct shared *sh = (struct shared *)shmat(id, NULL, 0);
    int fd[2];
    pipe(fd);

    printf("Provide Your Input From Given Options:\n");
    printf("1. Type a to Add Money\n");
    printf("2. Type w to Withdraw Money\n");
    printf("3. Type c to Check Balance\n");

    char buf[128];
    if (!fgets(buf, sizeof(buf), stdin)) { buf[0] = 'o'; buf[1] = '\0'; }
    trim(buf);

    sh->b = 1000;
    memset(sh->sel, 0, sizeof(sh->sel));
    strncpy(sh->sel, buf, sizeof(sh->sel) - 1);
    printf("Your selection: %s\n\n", sh->sel);

    pid_t p = fork();
    if (p == 0) {
        close(fd[0]);
        char c = sh->sel[0];
        long amt;

        if (c == 'a') {
            printf("Enter amount to be added:\n");
            if (scanf("%ld", &amt) != 1) amt = -1;
            if (amt > 0) {
                sh->b += (int)amt;
                printf("Balance added successfully\n");
                printf("Updated balance after addition:\n%d\n", sh->b);
            } else {
                printf("Adding failed, Invalid amount\n");
            }
        } else if (c == 'w') {
            printf("Enter amount to be withdrawn:\n");
            if (scanf("%ld", &amt) != 1) amt = -1;
            if (amt > 0 && amt < sh->b) {
                sh->b -= (int)amt;
                printf("Balance withdrawn successfully\n");
                printf("Updated balance after withdrawal:\n%d\n", sh->b);
            } else {
                printf("Withdrawal failed, Invalid amount\n");
            }
        } else if (c == 'c') {
            printf("Your current balance is:\n%d\n", sh->b);
        } else {
            printf("Invalid selection\n");
        }

        const char *msg = "Thank you for using\n";
        write(fd[1], msg, strlen(msg));
        close(fd[1]);
        shmdt(sh);
        _exit(0);
    } else {
        close(fd[1]);
        wait(NULL);
        char r[128] = {0};
        ssize_t n = read(fd[0], r, sizeof(r) - 1);
        if (n > 0) { r[n] = '\0'; printf("%s", r); }
        close(fd[0]);
        shmdt(sh);
        shmctl(id, IPC_RMID, NULL);
    }

    return 0;
}
