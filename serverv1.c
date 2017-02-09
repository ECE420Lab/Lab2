#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

char** theArray;

void *ServerEcho(void *args)  //this is the thread function
{
        int clientFileDescriptor=(int)args;                                                                         
        char pos_buff[20];
        char op_buff[20];

        read(clientFileDescriptor,pos_buff,20);
        int pos = atoi(pos_buff)-1;
        printf("received position %d from %d\n",pos,clientFileDescriptor);

        //write(clientFileDescriptor,"ACK pos",20);
        //printf("sending ack (pos) to client \n");
/*
        read(clientFileDescriptor,op_buff,20);

        

        int op = (int)op - 1;
        if (op<95){
        	printf("recieved a write operation \n");
        }
        else{
        	printf("received a read operation \n");
        }

        write(clientFileDescriptor,"ACK operation",20);
        printf("sending ack (op) to client \n");
*/
        close(clientFileDescriptor);
}

// void* serviceClient(void* args){

// 	int clientFileDescriptor=(int)args;                                                                         
//     char pos_buff[20];
//     char op_buff[20];
//     // char msg[100];

//     read(clientFileDescriptor,str,100);
//     int pos = pos_buff;

//     read(clientFileDescriptor,str,100);
//     char operation = op_buff;

//     if(operation=='r'){
//     	readArray(pos);
//     }
//     else{
//     	read(clientFileDescriptor,str,100);
//     	writeArray(pos,msg);
//     }


//     close(clientFileDescriptor);
// }


// int readArray(int pos){

// }

// int writeArray(int pos){

// }


int main(int argc, char* argv[]){

	if (argc > 3){
		printf("Error: too many inputs.\n");
		return 1;
	}

	int n = atoi(argv[2]);
    /*
	if ((n!=10)||(n!=100)||(n!=1000)||(n!=10000)){
		printf("Wrong size of array.\n");
		return 1;
	}*/


	theArray=malloc(n*sizeof(char));
	for (int i=0; i<n;i++){
		theArray[i]=malloc(100*sizeof(char)); 
		theArray[i]="the initial value"; //fix this
	}

	struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    pthread_t t[100];

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3000;//atoi(argv[1]); //user input is assumed to be correct
    sock_var.sin_family=AF_INET;

    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0){
    		printf("socket has been created\n");
            listen(serverFileDescriptor,2000);  
            while(1)        //loop infinity 
                {
                        for(int i=0;i<100;i++)      //can support 1000 clients at a time
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


	
}//main