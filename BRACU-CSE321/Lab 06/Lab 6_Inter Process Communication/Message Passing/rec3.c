#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct msg{
  long int type;                 
  char txt[6];
};

int main(){
	pid_t p_id;
	int msg_id;

	p_id=getpid();
	
	msg_id=msgget((key_t)51,0666|IPC_CREAT);

	if (msg_id==-1){
		perror("msg_id");
	}
	
	long int a1[]={2,4,5,3,1};
	int i;
	for(i=0;i<5;i++){
		sleep(1);
		struct msg s_data;
		
		if(msgrcv(msg_id,&s_data,sizeof(s_data),a1[i],IPC_NOWAIT)<0){
			perror("msgrcv");
		}
		printf("Data Received: %s, type %ld\n",s_data.txt,s_data.type);
		
	}
	msgctl(msg_id,IPC_RMID,0);

	return 0;
}
