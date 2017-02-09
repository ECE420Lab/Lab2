#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
//#include"mylib_rwlock.h"

typedef struct {
	int readers;
	int writer;
	pthread_cond_t readers_proceed;
	pthread_cond_t writer_proceed;
	int pending_writers;
	pthread_mutex_t read_write_lock;
} mylib_rwlock_t;

char** theArray;
pthread_mutex_t mutex;
mylib_rwlock_t rwlock;
void* ServerEcho(void *args);


void mylib_rwlock_init (mylib_rwlock_t *l) {
	l -> readers = l -> writer = l -> pending_writers = 0;
	pthread_mutex_init(&(l -> read_write_lock), NULL);
	pthread_cond_init(&(l -> readers_proceed), NULL);
	pthread_cond_init(&(l -> writer_proceed), NULL);
}

void mylib_rwlock_rlock(mylib_rwlock_t *l) {
	/* if there is a write lock or pending writers, perform condition wait, else increment count of readers and grant read lock */

	pthread_mutex_lock(&(l -> read_write_lock));
	while ((l -> pending_writers > 0) || (l -> writer > 0))
		pthread_cond_wait(&(l -> readers_proceed),
			&(l -> read_write_lock));
	l -> readers ++;
	pthread_mutex_unlock(&(l -> read_write_lock));
}

void mylib_rwlock_wlock(mylib_rwlock_t *l) {
	/* if there are readers or writers, increment pending writers count and wait. On being woken, decrement pending writers count and increment writer count */
	
	pthread_mutex_lock(&(l -> read_write_lock));
	while ((l -> writer > 0) || (l -> readers > 0)) {
		l -> pending_writers ++;
		pthread_cond_wait(&(l -> writer_proceed),
			&(l -> read_write_lock));
    	l -> pending_writers --;
	}
	l -> writer ++;
	pthread_mutex_unlock(&(l -> read_write_lock));
}

void mylib_rwlock_unlock(mylib_rwlock_t *l) {
	/* if there is a write lock then unlock, else if there are read locks, decrement count of read locks. If the count is 0 and there is a pending writer, let it through, else if there are pending readers, let them all go through */

	pthread_mutex_lock(&(l -> read_write_lock));
	if (l -> writer > 0)
		l -> writer = 0;
	else if (l -> readers > 0)
		l -> readers --;
	pthread_mutex_unlock(&(l -> read_write_lock));
	if ((l -> readers == 0) && (l -> pending_writers > 0))
		pthread_cond_signal(&(l -> writer_proceed));
	else if (l -> readers > 0)
		pthread_cond_broadcast(&(l -> readers_proceed));
}


int main(){
	
	mylib_rwlock_init(&rwlock);

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
        }

    }
    else{
    	printf("socket creation failed \n");
    	return 1;
    }

    return 0;

} //end of main

void* ServerEcho(void* args){
	long clientFileDescriptor = (long) args;
	char buff[20];

	read(clientFileDescriptor,buff,20);

	int pos;
	char operation;
	
	//printf("The server buffer contains position %d, and operation %c from %ld  \n",buff,clientFileDescriptor);

	sscanf(buff, "%d %c", &pos,&operation);
	printf("a Server thread recieved position %d and operation %c from %ld \n",pos,operation,clientFileDescriptor);

	if(operation=='r'){
		readRequest(pos);	
	}
	else if(operation=='w'){
		writeRequest(pos);		
	}
	else{
		printf("there has been an error in recieving operation and position from the client \n");
		//pthread exit	
	}
	
	

	return NULL;
}

void readRequest(int pos){
	

}

void writeRequest(int pos){
	
}




