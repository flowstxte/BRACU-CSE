#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int t_id[2]={1,2};
void *t_func(int *id);
int count=0;
int main(){
	pthread_t t[2];
	pthread_create(&t[0],NULL,(void *)t_func,&t_id[0]);
	pthread_create(&t[1],NULL,(void *)t_func,&t_id[1]);
	
	for(int i=0;i<2;i++){
		pthread_join(t[i],NULL);
	}
	
	printf("Total count: %d\n",count);
	return 0;
}
void *t_func(int *id){
	printf("Entered in Thread %d...\n",*id);
	for(int i=0;i<100000;i++){
		count++;
		//sleep(1);
	}
	
}
