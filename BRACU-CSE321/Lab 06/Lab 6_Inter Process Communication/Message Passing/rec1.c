#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

struct msg{
	long int type;
	char txt[200];
};

int main(){
	pid_t p_id;
	int msg_id;
	int rcv;
	struct msg r_data;
	char buff[200];
	p_id=getpid();
	msg_id=msgget((key_t)12,0666|IPC_CREAT);
	//int msg_to_rec=2;
	if (msg_id==-1){
		printf("Error\n");
	}

	printf("Read:\n");
	if(msgrcv(msg_id,(void *)&r_data,sizeof(r_data.txt),-4,IPC_NOWAIT)<0){
		perror("Error");
	}

	printf("Data received:\n%s\n",r_data.txt);

	msgctl(msg_id,IPC_RMID,0);

	return 0;
}

