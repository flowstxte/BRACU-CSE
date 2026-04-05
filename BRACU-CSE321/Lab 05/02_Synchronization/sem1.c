#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int t_id[4]={1,2,3,4};
void *t_func(int *id);
int count=0;
sem_t s;

int main(){
	pthread_t t[4];
	sem_init(&s,0,1);
	
	for(int i=0;i<4;i++){
		pthread_create(&t[i],NULL,(void *)t_func,&t_id[i]);
	}
	
	
	for(int i=0;i<4;i++){
		pthread_join(t[i],NULL);
	}
	
	sem_destroy(&s);
	printf("Total count: %d\n",count);
	return 0;
}
void *t_func(int *id){
	printf("Entered in Thread %d...\n",*id);
	for(int i=0;i<100000;i++){
		sem_wait(&s);
		count++;
		sem_post(&s);
	}
	
}
