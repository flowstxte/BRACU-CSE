#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *t_func1(void *arg);
void *t_func2(void *arg1);
int t_id[]={1,2};
int a=10;
int b=5;

int main(){
	int a1[]={t_id[0],a,b};
	int a2[]={t_id[1],a,b};
	pthread_t t1;
	pthread_t t2;
	pthread_create(&t1,NULL,(void *)t_func1,(void *)a1);
	pthread_create(&t2,NULL,(void *)t_func2,(void *)a2);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	
	return 0;
}

void *t_func1(void *arg){
	int *x=arg;
	printf("Entered in Thread :%d\n",x[0]);
	sleep(1);
	int add=x[1]+x[2];
	printf("ADD :%d\n",add);
	printf("Addition Done by Thread %d...\n",x[0]);
}

void *t_func2(void *arg1){
	int *y=arg1;
	printf("Entered in Thread :%d\n",y[0]);
	sleep(1);
	int sub=y[1]-y[2];
	printf("SUB :%d\n",sub);
	printf("Substraction Done by Thread %d...\n",y[0]);
}