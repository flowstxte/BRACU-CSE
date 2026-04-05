#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

struct msg
{
    long int type;
    char txt[6];
};
static void trim(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[n - 1] = '\0';
}
int main(void) {
    int mqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    const long T_LT_O = 1, T_O_L = 2, T_O_M = 3, T_M_L = 4;

    printf("Please enter the workspace name:\n");
    char ws[64];
    if (!fgets(ws, sizeof(ws), stdin)) {
        perror("fgets");
        msgctl(mqid, IPC_RMID, NULL);
        return 1;
    }
    trim(ws);
    if (strcmp(ws, "cse321") != 0) {
        printf("Invalid workspace name\n");
        msgctl(mqid, IPC_RMID, NULL);
        return 0;
    }

    struct msg msg1;
    msg1.type = T_LT_O;
    memcpy(msg1.txt, "cse321", 6);
    msgsnd(mqid, &msg1, sizeof(msg1.txt), 0);
    printf("Workspace name sent to otp generator from log in: %.*s\n\n", (int)sizeof(msg1.txt), msg1.txt);

    pid_t p_otp = fork();
    if (p_otp == 0) {
        struct msg rcv;
        msgrcv(mqid, &rcv, sizeof(rcv.txt), T_LT_O, 0);
        printf("OTP generator received workspace name from log in: %.*s\n\n", (int)sizeof(rcv.txt), rcv.txt);

        char otp[6];
        snprintf(otp, sizeof(otp), "%d", (int)getpid());

        struct msg s1;
        s1.type = T_O_L;
        memcpy(s1.txt, otp, 6);
        msgsnd(mqid, &s1, sizeof(s1.txt), 0);
        printf("OTP sent to log in from OTP generator: %.*s\n", (int)sizeof(s1.txt), s1.txt);

        struct msg s2;
        s2.type = T_O_M;
        memcpy(s2.txt, otp, 6);
        msgsnd(mqid, &s2, sizeof(s2.txt), 0);
        printf("OTP sent to mail from OTP generator: %.*s\n", (int)sizeof(s2.txt), s2.txt);

        pid_t p_mail = fork();
        if (p_mail == 0) {
            struct msg rcv2;
            msgrcv(mqid, &rcv2, sizeof(rcv2.txt), T_O_M, 0);
            printf("Mail received OTP from OTP generator: %.*s\n", (int)sizeof(rcv2.txt), rcv2.txt);

            struct msg s3;
            s3.type = T_M_L;
            memcpy(s3.txt, rcv2.txt, 6);
            msgsnd(mqid, &s3, sizeof(s3.txt), 0);
            printf("OTP sent to log in from mail: %.*s\n", (int)sizeof(s3.txt), s3.txt);
            _exit(0);
        } else {
            wait(NULL);
            _exit(0);
        }
    } else {
        struct msg g1;
        msgrcv(mqid, &g1, sizeof(g1.txt), T_O_L, 0);
        printf("Log in received OTP from OTP generator: %.*s\n", (int)sizeof(g1.txt), g1.txt);

        struct msg g2;
        msgrcv(mqid, &g2, sizeof(g2.txt), T_M_L, 0);
        printf("Log in received OTP from mail: %.*s\n", (int)sizeof(g2.txt), g2.txt);

        if (memcmp(g1.txt, g2.txt, 6) == 0) printf("OTP Verified\n");
        else printf("OTP Incorrect\n");

        msgctl(mqid, IPC_RMID, NULL);
        wait(NULL);
    }

    return 0;
}
