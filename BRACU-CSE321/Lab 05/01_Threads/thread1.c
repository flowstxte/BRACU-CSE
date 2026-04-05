#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
void *funcThread(void *arg);
int main(){
	pthread_t t1;
	pthread_create(&t1,NULL,funcThread,NULL);
	func1();
	pthread_join(t1,NULL);
	
	return 0;
}
void func1(){
	printf("Entered func1:\n");
	for(int i=0;i<5;i++){
		printf("func1: %d\n",i);
		sleep(1);
	}
	printf("Done with func 1....\n");

}
void *funcThread(void *arg){
	printf("Entered thread1:\n");
	for(int i=0;i<5;i++){
		printf("thread: %d\n",i);
		sleep(1);
	}
	printf("Done with thread 1....\n");
}

