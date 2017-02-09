#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>


int* seed;
// struct sockaddr_in sock_var;
int aSize;

void *connectServer(void *args){
	char P[20],O[20];
	char pACK[20],oACK[20];
	int pos;
	int randNum;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	long rank = (long)args;

	//printf("this is a server thread\n");
	struct sockaddr_in sock_var;
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("hi there im thread %d im sending %d\n",clientFileDescriptor,pos);
		pos = rand_r(&seed[rank]) % aSize +1;
		printf("%d\n",pos);
		sprintf(P, "%d", pos);

		//randNum = rand_r(&seed[rank]) % 100 +1;
		//sprintf(O, "%d", randNum);

		write(clientFileDescriptor,P,20);
		//read(clientFileDescriptor,pACK,20);
		printf("Position transmitted!%d\n",clientFileDescriptor);
/*
		write(clientFileDescriptor,O,20);
		read(clientFileDescriptor,oACK,20);
		printf("Operation transmitted!%d\n",clientFileDescriptor);
*/
		

		/*
		read(clientFileDescriptor,str_ser,20);
		printf("String from Server: %s\n",str_ser);
		scanf("%s",str_clnt);
		write(clientFileDescriptor,str_clnt,20);
		read(clientFileDescriptor,str_ser,20);
		printf("String from Server: %s\n",str_ser);
		*/
		//close(clientFileDescriptor);
	}
	else{
		printf("socket creation failed%d\n",clientFileDescriptor);
	}
	close(clientFileDescriptor);
}

int main(int argc, char* argv[])
{
	int threadCount = 100;
	pthread_t t[100];
	//char str_clnt[20],str_ser[20];

	if(argc != 3){
		printf("wrong input, please try again!\n");
	}

	aSize = atoi(argv[2]);
	/*
	if ((aSize!=10)||(aSize!=100)||(aSize!=1000)||(aSize!=10000)){
		printf("Wrong size of array.\n");
		return 1;
	}*/
	

	//sock_var = malloc((sizeof(sock_var));

	// sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	// sock_var.sin_port=3000;
	// sock_var.sin_family=AF_INET;


	seed = malloc(threadCount*sizeof(int)); 
	int p;
	for(p = 0;p<threadCount;p++){
		seed[p] = p;
		//printf("p=%d\n",p);
		pthread_create(&t[p],NULL,connectServer,(void *)p);
	}
	

	return 0;
}

