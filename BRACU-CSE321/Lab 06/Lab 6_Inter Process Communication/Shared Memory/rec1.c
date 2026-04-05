#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main(){
	pid_t p_id;
	p_id=getpid();
	void *s_m;
	char buff[200];
	int sm_id;
	sm_id=shmget((key_t)101,1024,0666);
	printf("ID of receiver process: %d\n",p_id);
	printf("Key of SM: %d\n",sm_id);
	s_m=shmat(sm_id,NULL,0); //process attached to shared memory
	printf("Process attached at %p with id %d\n",s_m,p_id);
	printf("Read data after receiving:\n");
	printf("Reading complete by process with id %d\n",p_id);
	printf("Data read: %s\n",(char *)s_m);
	
	shmctl(sm_id,IPC_RMID, NULL);

	return 0;
}

