#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include "timer.h"

//gcc -std=gnu99  .c -o s -lpthread -Wall

char** theArray;
int aSize=100;
int usr_port;
pthread_rwlock_t* rwlockArray;

void* ServerEcho(void *args);

int main(int argc, char * argv []){

	int n = atoi(argv[2]);
	usr_port = atoi(argv[1]);

	if( (n==10)||(n==100)||(n==1000)||(n==10000) ){          //default:100
		aSize=n;	
	}

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=usr_port;
    sock_var.sin_family=AF_INET;

    pthread_t* thread_handles;
    double start,end,time;

    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created \n");
        listen(serverFileDescriptor,2000); 

        while(1){
               thread_handles = malloc(1000*sizeof(pthread_t));
               theArray=malloc(aSize*sizeof(char*)); 
               rwlockArray=malloc(aSize*sizeof(pthread_rwlock_t));              
               for (int i=0; i<aSize;i++){
                    theArray[i]=malloc(100*sizeof(char)); //use 100 for msg
                    snprintf(theArray[i],100, "%s%d%s", "String ", i, ": the initial value" );
                    pthread_rwlock_init(&rwlockArray[i],NULL);
               }   
	        

            GET_TIME(start);

            for(int i=0;i<1000;i++){
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
               // printf("Connect to client %d \n",clientFileDescriptor);                     //PRINT

                pthread_create(&thread_handles[i],NULL,ServerEcho,(void*)clientFileDescriptor);
            }   
            for(int i = 0;i<1000;i++){
                pthread_join(thread_handles[i],NULL);
            }

            GET_TIME(end);
            time = end-start;
            printf(" %f \n", time);
	
    		//printf("All threads have joined. Checking Array \n");
    		//for(int i=0;i<aSize;i++){
    		//	printf("%s\n",theArray[i]);
    		//}


           	    for (int i=0; i<aSize;i++){
                    	free(theArray[i]);
                	pthread_rwlock_destroy(&rwlockArray[i]);
             	}   
                free(rwlockArray);
            	free(theArray);
            	free(thread_handles);
           
        }//end of infinite while loop

    }
    else{
        printf("socket creation failed \n");
        return 1;
    }


    return 0;

} //end of main

void* ServerEcho(void* args){
    long clientFileDescriptor = (long) args;
    char buff[100];

    read(clientFileDescriptor,buff,100);

    int pos;
    char operation;
    int CSerror;

    sscanf(buff, "%d %c", &pos,&operation);
    //printf("Server thread recieved position %d and operation %c from %d \n",pos,operation,clientFileDescriptor); //PRINT 

    if(operation=='r'){
        char msg[100];
        pthread_rwlock_rdlock(&rwlockArray[pos]);
        	CSerror=snprintf(msg,100, "%s", theArray[pos] );
        pthread_rwlock_unlock(&rwlockArray[pos]);

        if(CSerror<0){
            printf("ERROR: could not read from position: %d \n", pos);
            sprintf(msg, "%s %d","Error reading from pos: ", pos );     
        }

        write(clientFileDescriptor,msg,100);
        close(clientFileDescriptor);
    }
    else if(operation=='w'){
	    char msg[100];
        snprintf(msg,100, "%s%d%s","String ", pos, " has been modified by a write request" );	

	
        pthread_rwlock_wrlock(&rwlockArray[pos]);
        	CSerror=snprintf(theArray[pos],100, "%s", msg );
        pthread_rwlock_unlock(&rwlockArray[pos]);

        if(CSerror<0){
            printf("ERROR: could not write position: %d \n", pos);
            sprintf(msg, "%s %d","Error writing to pos: ", pos );     
        }

        write(clientFileDescriptor,msg,100);   
        close(clientFileDescriptor);    
    }
    else{
        printf("ERROR: could not communicate with client \n");
    }
   
    return NULL;
}
