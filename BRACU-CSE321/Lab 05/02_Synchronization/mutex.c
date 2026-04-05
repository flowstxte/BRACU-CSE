#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int t_id[2]={1,2};
void *t_func(int *id);
int count=0;
pthread_mutex_t mutex;
int main(){
	pthread_t t[2];
	pthread_mutex_init(&mutex,NULL);
	pthread_create(&t[0],NULL,(void *)t_func,&t_id[0]);
	pthread_create(&t[1],NULL,(void *)t_func,&t_id[1]);
	
	for(int i=0;i<2;i++){
		pthread_join(t[i],NULL);
	}
	
	pthread_mutex_destroy(&mutex);
	printf("Total count: %d\n",count);
	return 0;
}
void *t_func(int *id){
	printf("Entered in Thread %d...\n",*id);
	for(int i=0;i<100000;i++){
		pthread_mutex_lock(&mutex);
		count++;
		//sleep(1);
		pthread_mutex_unlock(&mutex);
	}
	
}
