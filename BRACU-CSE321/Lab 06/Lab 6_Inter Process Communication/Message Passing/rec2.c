#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg{
	int type;
	char txt[200];
};

int main(){
	pid_t p_id;
	int msg_id;
	struct msg s_data;
	p_id=getpid();
	msg_id=msgget((key_t)123,0666|IPC_CREAT);
	int msg_to_rec=0;
	int flag=1;
	if (msg_id==-1){
		printf("Error\n");
	}
	while(flag){
		printf("Read:\n");
		msgrcv(msg_id,(void *)&s_data,sizeof(s_data.txt),msg_to_rec,IPC_NOWAIT);
		printf("Msg received: %s\n",s_data.txt);
		if(strncmp(s_data.txt,"done",4)==0){
			flag=0;
		}
	}
	msgctl(msg_id,IPC_RMID,0);


	return 0;
}

