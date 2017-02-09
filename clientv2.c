#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

int* seed;
int aSize = 100;

void *connectServer(void *args){
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	long rank = (long)args;
	char buff[20];
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	
	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		int pos = rand_r(&seed[rank]) % aSize ;
		//sprintf(buff, "%d", pos);

		int oper = rand_r(&seed[rank]) % 100 ;
		if(oper<95){
			sprintf(buff,"%d %c",pos,'r');		
		}
		else{
			sprintf(buff,"%d %c",pos,'w');		
		}
		
		//printf("The client buffer contains position %d, and operation %c \n",buff);
		write(clientFileDescriptor,buff,20);
		
	}
	else{
		printf("socket connection failed\n");
	}
	close(clientFileDescriptor);
}

int main()
{
	long thread;
	pthread_t thread_handles[1000];

	seed = malloc(1000*sizeof(int)); 

	for (thread = 0; thread < 1000; thread++) {
		seed[thread] = thread;
		pthread_create(&thread_handles[thread], NULL, connectServer, (void*) thread);  			
	}
	for (thread = 0; thread < 1000; thread++) {
		pthread_join(thread_handles[thread], NULL);  			
	}
	return 0;
}
