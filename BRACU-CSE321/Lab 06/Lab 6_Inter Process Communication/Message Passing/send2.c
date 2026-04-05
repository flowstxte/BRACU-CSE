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
	char buff[200];
	p_id=getpid();
	msg_id=msgget((key_t)123,0666|IPC_CREAT);
	int flag=1;
	if (msg_id==-1){
		printf("Error\n");
	}
	while(flag){
		printf("Write:\n");
		fgets(buff,sizeof(buff),stdin);
		s_data.type=1;
		strcpy(s_data.txt,buff);
		if(msgsnd(msg_id,(void *)&s_data,sizeof(s_data.txt),0)==-1){
			printf("Error while sending msg\n");
		}
		if(strncmp(buff,"done",4)==0){
			flag=0;
		}
	}


	return 0;
}

