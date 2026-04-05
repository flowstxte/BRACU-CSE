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
	int snt;

	p_id=getpid();
	
	msg_id=msgget((key_t)51,0666|IPC_CREAT);

	if (msg_id==-1){
		perror("msg_id");
	}
	long int a[]={1,2,3,4,5};
	char *buff[]={"abc","def","ijk","lmn","opq"};
	int i;
	for(i=0;i<5;i++){
		struct msg s_data;
		s_data.type=a[i];
		strcpy(s_data.txt,buff[i]);
		snt=msgsnd(msg_id,&s_data,sizeof(s_data),IPC_NOWAIT);
		if(snt==-1){
			perror("msgsnt");
		}
		printf("Data Sent: %s, type %ld\n",s_data.txt,s_data.type);
		
	}

	return 0;
}

