#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int *func_thread(int *v);
int *t_ret;

int main(){
	pthread_t t1;
	int n=5;
	pthread_create(&t1,NULL,func_thread,&n);
	pthread_join(t1,&t_ret);
	printf("Thread returned: %d\n",t_ret);
	
	return 0;
}
int *func_thread(int *v){
    *v=*v*5;
	return *v;
}
