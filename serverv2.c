#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

void *ServerEcho(void *args)
{
	int clientFileDescriptor=(int)args;
	printf("%d\n", clientFileDescriptor);

	close(clientFileDescriptor);
}


int main()
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;
	pthread_t t[1000];

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("socket has been created\n");
		listen(serverFileDescriptor,2000); 
		while(1)        //loop infinity
		{
			for(i=0;i<1000;i++)      //can support 20 clients at a time
			{
				clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
				printf("Connected to client %d\n",clientFileDescriptor);
				pthread_create(&t[i],NULL,ServerEcho,(void *)clientFileDescriptor);
			}
		}
		close(serverFileDescriptor);
	}
	else{
		printf("nsocket creation failed\n");
	}
	return 0;
}