#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *func_thread(int *n);
void *t_ret;
int num=5;

int main(){
	pthread_t t1;
	pthread_create(&t1,NULL,(void *)func_thread,&num);
	pthread_join(t1,&t_ret);
	printf("Thread returned: %d\n",(int *)t_ret);
	
	return 0;
}

void *func_thread(int *n){
	printf("Entered in Thread:\n");
	if(*n % 2==0){
		pthread_exit(*n * *n);
		printf("Operation completed\n");
	}
	else{
		pthread_exit(*n * *n * *n);
		printf("Operation completed\n");
	}
}
