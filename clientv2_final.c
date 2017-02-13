#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

int* seed;
int aSize=100;
int usr_port;

void *connectServer(void *args){
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	long rank = (long)args;
	char buff[100];
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=usr_port;
	sock_var.sin_family=AF_INET;

	
	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		int pos = rand_r(&seed[rank]) % aSize ;
		int oper = rand_r(&seed[rank]) % 100 ;

		if(oper<95){
			if(snprintf(buff,100,"%d %c",pos,'r')<0){
				printf("ERROR: client could not write to buffer. \n");
				return(NULL);			
			}		
		}
		else if(oper>=95){
			if(snprintf(buff,100,"%d %c",pos,'w')<0){
				printf("ERROR: client could not write to buffer. \n");
				return(NULL);			
			}	
		}
		else{
			if(snprintf(buff,100,"%d %c",pos,'e')<0){
				printf("ERROR: client could not write to buffer. \n");
				return(NULL);			
			}	
		}

		write(clientFileDescriptor,buff,100);

		//handle server request result
		char result[100];
		//char msg[100];
		read(clientFileDescriptor,result,100);
		//sscanf(result, "%s", msg);
		printf("%s\n",result);
		
	}
	else{
		printf("socket connection failed\n");
	}
	close(clientFileDescriptor);
	return(NULL);
}

int main(int argc, char * argv []) //first argument: server second argument: size of array (n) 10, 100, 1000, and 10,000
{
	int n = atoi(argv[2]);
	usr_port = atoi(argv[1]);

	if( (n==10)||(n==100)||(n==1000)||(n==10000) ){ //default:100
		aSize=n;	
	}
	

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
	free(seed);
	return 0;
}
