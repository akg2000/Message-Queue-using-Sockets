#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
// #include <threads.h>
#define PORT 8080


pthread_mutex_t writer_lock;

struct Queue* q;


struct LLNode{
	char message[500];
	struct LLNode* next;
	pthread_mutex_t reader_lock;
};
struct Queue{
	struct LLNode *front;
	struct LLNode *rear;
	int size;
};
void editspecific(char *msg,int index){
	struct LLNode* temp = (struct LLNode*)malloc(sizeof(struct LLNode));
	temp=q->front;
	char snd[1000];
	if(temp==NULL){
		printf("Queue is Empty\n");
		return;
	}
	if(index>(q->size+1)){
		printf("Invalid Index\n");
		return;
	}
	int i=1;
	while(i!=index){
		pthread_mutex_lock(&(temp->reader_lock));
		i++;
		temp=temp->next;
		pthread_mutex_unlock(&(temp->reader_lock));
	}
	memset(snd,'\0',sizeof(snd));
	pthread_mutex_lock(&(temp->reader_lock));
	strcpy(temp->message,msg);
	printf("%s\n",temp->message);
	pthread_mutex_unlock(&(temp->reader_lock));
	return;
}
void enQueue(char *msg) { 
	// usleep(10000000);
	struct LLNode* temp = (struct LLNode*)malloc(sizeof(struct LLNode)); 
	temp->next=NULL;
	strcpy(temp->message,msg);
	printf("stored : %s\n",temp->message);
	pthread_mutex_lock(&writer_lock);
	if(q->rear==NULL){
		q->rear=q->front=temp;
	}
	else{
		q->rear->next=temp;
		q->rear=q->rear->next;
	}
	q->size=(q->size+1);
	pthread_mutex_unlock(&writer_lock);
    return;
} 
void deQueue(int curr){ 
	char snd[1000];
	memset(snd,'\0',sizeof(snd));
	pthread_mutex_lock(&writer_lock);
	if((q->front)==NULL){
		sprintf(snd,"Queue is Empty");
		if(send(curr,snd,strlen(snd),0) < 0) {
			perror("sending failure");
		}
		return;
	}
	sprintf(snd,"Dequeued Element : %s\n",q->front->message);
	printf("Dequeued Element : %s\n",q->front->message);
	q->front=q->front->next;
	q->size=(q->size-1);
	printf("%s\n",snd);
	pthread_mutex_unlock(&writer_lock);
	printf("ahahahahahh\n");
    return;
} 
void displayall(char *msg,int curr){
	struct LLNode* temp = (struct LLNode*)malloc(sizeof(struct LLNode));
	temp=q->front;
	char snd[1000];
	if(temp==NULL){
		memset(snd,'\0',sizeof(snd));
		sprintf(snd,"Queue is Empty");
		if(send(curr,snd,strlen(snd),0) < 0) {
			perror("sending failure");
		}		
		return;
	}
	int i=1;
	while(temp!=NULL){
		memset(snd,'\0',sizeof(snd));
		pthread_mutex_lock(&(temp->reader_lock));
		sprintf(snd,"Message %d : %s\n",i,temp->message);
		if(send(curr,snd,strlen(snd),0) < 0) {
			perror("sending failure");
		}
		pthread_mutex_unlock(&(temp->reader_lock));
		i++;
		temp=temp->next;
	}
	printf("printing successfull \n");
	return;
}
void displayspecific(int index,int curr){
	struct LLNode* temp = (struct LLNode*)malloc(sizeof(struct LLNode));
	temp=q->front;
	char snd[1000];
	if(temp==NULL){
		printf("Queue is Empty\n");
		return;
	}
	if(index>(q->size+1)){
		printf("Invalid Index\n");
		return;
	}
	int i=1;
	while(i!=index){
		pthread_mutex_lock(&(temp->reader_lock));
		// printf("Message %d : %s\n",i,temp->message);
		i++;
		temp=temp->next;
		pthread_mutex_unlock(&(temp->reader_lock));
	}
	memset(snd,'\0',sizeof(snd));
	pthread_mutex_lock(&(temp->reader_lock));
	sprintf(snd,"Message : %s\n",temp->message);
	printf("%s\n",temp->message);
	pthread_mutex_unlock(&(temp->reader_lock));
	if(send(curr,snd,strlen(snd),0) < 0) {
		perror("sending failure");
	}
	return;
}
int isEmpty(struct Queue* q){  return (q->size == 0); } 
struct Queue* createQueue(){ 
    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL;
    q->size = 0;  
    return q; 
} 

struct Node* start =NULL; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int clients[100];
int n = 0;

void slice_str(const char * str, char * buffer, size_t start, size_t end){
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}
struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};
struct Node{
	struct client_info client;
	struct Node* next;
	struct Node* prev;
};
void *recieve_message(void *sock){
	struct client_info cl = *((struct client_info *)sock);
	char msg[500];
	int len,i,j;
	char msg2[500];
	while((len = recv(cl.sockno,msg,500,0)) > 0) {
		// msg[len] = '\0';
		// char * temp = msg[0];
		if(strchr(msg,'#')!=NULL){
			// printf("lllll\n");
			slice_str(msg,msg,1,strlen(msg)-1);
			int result = atoi( msg );
			printf("%d\n",result );
			displayspecific(result,cl.sockno);
		}
		else if(strchr(msg,'@')!=NULL){
			// printf("lllll\n");
			slice_str(msg,msg,1,strlen(msg)-1);
			// int result = atoi( msg );
			enQueue(msg);
		}
		else if(strcmp(msg,"readall")==0){
			displayall(msg,cl.sockno);
		}
		else if(strcmp(msg,"dequeue")==0){
			deQueue(cl.sockno);
		}
		else{
			slice_str(msg,msg2,0,1);
			printf("msg2 %s\n",msg2 );
			int result = atoi( msg2 );
			printf("%d\n",result );
			slice_str(msg,msg,1,strlen(msg)-1);
			editspecific(msg,result);
		}
		memset(msg,'\0',sizeof(msg));
	}
	pthread_mutex_lock(&mutex);
	printf("client at socket no. %d disconnected\n",cl.sockno);
	for(i = 0; i < n; i++) {
		if(clients[i] == cl.sockno) {
			j = i;
			for(j=i;j<(n-1);j++){
				clients[j] = clients[j+1];
				j++;
			}
		}
	}
	n--;
	printf("Total no of clients connected : %d\n",n);
	pthread_mutex_unlock(&mutex);
}
int main(){
	q = createQueue();
	struct sockaddr_in my_addr,their_addr;
	int server_sock,client_sock,len;
	socklen_t their_addr_size;
	pthread_t sendt,recvt;
	char msg[500];
	struct client_info cl;
	char ip[INET_ADDRSTRLEN];
	server_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);
	if(bind(server_sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) != 0) {
		perror("binding on socket unsuccessful");
		exit(1);
	}
	if(listen(server_sock,5) != 0) {
		perror("listen on socket unsuccessful");
		exit(1);
	}
	start = (struct Node*)malloc(sizeof(struct Node));
	struct Node* temp = start;

	while(1) {
		if((client_sock = accept(server_sock,(struct sockaddr *)&their_addr,&their_addr_size)) < 0) {
			perror("accept unsuccessful");
			exit(1);
		}
		pthread_mutex_lock(&mutex);
		inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
		printf("client at socket no. %d connected\n",cl.sockno);
		cl.sockno = client_sock;
		strcpy(cl.ip,ip);
		temp->client.sockno = cl.sockno;
		strcpy(temp->client.ip , cl.ip);
		temp->next = (struct Node*)malloc(sizeof(struct Node));
		temp = temp->next;
		clients[n] = client_sock;
		n++;
		printf("Total no of clients connected : %d\n",n);
		pthread_create(&recvt,NULL,recieve_message,&cl);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}