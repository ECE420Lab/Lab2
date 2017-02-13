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
int aSize=100;
int usr_port=3000;

void* ServerEcho(void *args);

int main(int argc, char * argv []){
   
    pthread_rwlock_init(&rwlock,NULL);
    
    int n = atoi(argv[2]);
	usr_port = atoi(argv[1]);

	if( (n==10)||(n==100)||(n==1000)||(n==10000) ){ //default:100
		aSize=n;	
	}

    theArray=malloc(aSize*sizeof(char*));  
    for (int i=0; i<aSize;i++){
        theArray[i]=malloc(100*sizeof(char)); //use 100 for msg
        sprintf(theArray[i], "%s%d%s", "String ", i, ": the initial value" );
    }   

    struct sockaddr_in sock_var;
    int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    pthread_t* thread_handles;

    thread_handles = malloc(1000*sizeof(pthread_t));

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=usr_port;
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
    		for(int i=0;i<aSize;i++){
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
   
    int CSerror;

    sscanf(buff, "%d %c", &pos,&operation);
    printf("Server recieved position %d and operation %c from %ld \n",pos,operation,clientFileDescriptor);

    if(operation=='r'){
        char msg[100];
        pthread_rwlock_rdlock(&rwlock);
            CSerror=snprintf(msg,100, "%s", theArray[pos] );
        pthread_rwlock_unlock(&rwlock);

        if(CSerror<0){
            printf("ERROR: could not read from position %d\n",pos);
            snprintf(msg,100, "%s %d","Error reading pos: ", pos );     
        }

        write(clientFileDescriptor,msg,100);
    	close(clientFileDescriptor);
    }
    else if(operation=='w'){

        char msg[100];
        sprintf(msg, "%s%d%s","String ", pos, " has been modified by a write request" );

        pthread_rwlock_wrlock(&rwlock);
            CSerror=snprintf(theArray[pos],100, "%s",msg );
        pthread_rwlock_unlock(&rwlock);
    
        if(CSerror<0){
            printf("ERROR: could not write to array \n");
            sprintf(msg, "%s %d","Error writing to pos: ", pos );     
        }

        
        write(clientFileDescriptor,msg,100);     
    	close(clientFileDescriptor);  
    }
    else{
        printf("Error: could not communicate with client \n");
        return(NULL);   
    }
   
   

    return NULL;
}
