#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

//gcc -std=gnu99  server_pthread_rwLock.c -o s -lpthread -Wall

char** theArray;
pthread_rwlock_t rwlock;
void* ServerEcho(void *args);

int main(){
   
    pthread_rwlock_init(&rwlock,NULL);

    theArray=malloc(100*sizeof(char));  //hardcode array size for now
    for (int i=0; i<100;i++){

        theArray[i]=malloc(100*sizeof(char)); //use 100 for msg
            sprintf(theArray[i], "%s%d%s", "String ", i, ": the initial value" );
    }   

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    pthread_t* thread_handles;
    thread_handles = malloc(1000*sizeof(pthread_t));

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3000;
    sock_var.sin_family=AF_INET;

    if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("socket has been created \n");
        listen(serverFileDescriptor,2000); 

        while(1){
            for(int i=0;i<1000;i++){
                clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
                printf("Connect to client %d \n",clientFileDescriptor);
                pthread_create(&thread_handles[i],NULL,ServerEcho,(void*)clientFileDescriptor);
            }   
            for(int i = 0;i<1000;i++){
                pthread_join(thread_handles[i],NULL);
            }
	
    		printf("All threads have joined. Checking Array \n");
    		for(int i=0;i<100;i++){
    			printf("%s\n",theArray[i]);		
    		}
            pthread_rwlock_destroy(&rwlock);
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
   
    //printf("The server buffer contains position %d, and operation %c from %ld  \n",buff,clientFileDescriptor);

    sscanf(buff, "%d %c", &pos,&operation);
    printf("a Server thread recieved position %d and operation %c from %ld \n",pos,operation,clientFileDescriptor);

    if(operation=='r'){
        char msg[100];
        pthread_rwlock_rdlock(&rwlock);
        sprintf(msg, "%s", theArray[pos] );
        pthread_rwlock_unlock(&rwlock);

        write(clientFileDescriptor,msg,100);
    }
    else if(operation=='w'){

        pthread_rwlock_wrlock(&rwlock);
        //sprintf(theArray[pos], "%s%d%s","String ", pos, " has been modified by a write request\n" );

        pthread_rwlock_unlock(&rwlock);

        char msg[100];
        sprintf(msg, "%s%d%s","String ", pos, " has been modified by a write request \n" );
        write(clientFileDescriptor,msg,100);       
    }
    else{
        printf("there has been an error communicating with client \n");
        //pthread exit   
    }
   
   

    return NULL;
}