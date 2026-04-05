#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define STU 10
#define CHR 3

sem_t stu_sem, chr_sem;
pthread_mutex_t mtx;
int w_count = 0;
int s_count = 0;
int l_count = 0;

void *st(void *arg)
{
    while (1)
    {
        sem_wait(&stu_sem);
        pthread_mutex_lock(&mtx);
        w_count--;
        sem_post(&chr_sem);
        printf("A waiting student started getting consultation\n");
        printf("Number of students now waiting: %d\n", w_count);
        printf("ST giving consultation\n");
        pthread_mutex_unlock(&mtx);
        sleep(1);
        pthread_mutex_lock(&mtx);
        s_count++;
        printf("Student finished getting consultation and left\n");
        printf("Number of served students: %d\n", s_count);
        if (s_count + l_count >= STU)
        {
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
    }
    pthread_exit(NULL);
}

void *student(void *arg)
{
    int id = *((int *)arg);
    if (sem_trywait(&chr_sem) != 0)
    {
        pthread_mutex_lock(&mtx);
        l_count++;
        printf("No chairs remaining in lobby. Student %d Leaving.....\n", id);
        pthread_mutex_unlock(&mtx);
        pthread_exit(NULL);
    }
    pthread_mutex_lock(&mtx);
    w_count++;
    printf("Student %d started waiting for consultation\n", id);
    printf("Number of students now waiting: %d\n", w_count);
    pthread_mutex_unlock(&mtx);
    sem_post(&stu_sem);
    sleep(1);
    pthread_exit(NULL);
}

int main()
{
    pthread_t fac, studs[STU];
    int ids[STU];
    srand(time(NULL));
    sem_init(&stu_sem, 0, 0);
    sem_init(&chr_sem, 0, CHR);
    pthread_mutex_init(&mtx, NULL);
    pthread_create(&fac, NULL, st, NULL);
    for (int i = 0; i < STU; i++)
    {
        ids[i] = i;
        sleep(rand() % 2);
        pthread_create(&studs[i], NULL, student, &ids[i]);
    }
    for (int i = 0; i < STU; i++)
    {
        pthread_join(studs[i], NULL);
    }
    sem_post(&stu_sem);
    pthread_join(fac, NULL);
    sem_destroy(&stu_sem);
    sem_destroy(&chr_sem);
    pthread_mutex_destroy(&mtx);
    return 0;
}