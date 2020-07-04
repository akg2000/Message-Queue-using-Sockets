#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8080


char msg[500];
char username[100];
char res[600];
int len;
int my_sock,their_sock,their_addr_size;

void readermenu(){
	printf("Welcome user %s\n",username);
	printf("Options :-\n");
	printf("1) Read all Messages\n");
	printf("2) Read a specific Message.\n");
	printf("3) Dequeue Message.\n");
	printf("4) Show help\n");
	printf("5) Quit\n");
}
void writermenu(){
	printf("Welcome user %s\n",username);
	printf("Options :-\n");
	printf("1) Write Message\n");
	printf("2) Show help\n");
	printf("3) Quit\n");
	printf("4) edit some previous message with index\n");
}
void reader(){
	int x;
	while(1){
		printf("Option : ");
		scanf("%d",&x);
		if(x==5){	exit(0);	}
		else if(x==4){	readermenu();	}
		else if(x==1){
			// displayall();
			memset(res,'\0',sizeof(res));
			strcpy(res,"readall");
			len = write(my_sock,res,strlen(res));
		}
		else if(x==2){
			printf("Enter Message Number : ");
			scanf("%d",&x);
			memset(res,'\0',sizeof(res));
			sprintf(res,"#%d",x);
			len = write(my_sock,res,strlen(res));
		}
		else if(x==3){
			memset(res,'\0',sizeof(res));
			sprintf(res,"dequeue");
			len = write(my_sock,res,strlen(res));
		}
		else{
			printf("Invalid input try again\n");
		}
	}
}

void writer(){
	int x;
	char abc[100];
	while(1){
		printf("Option : ");
		// printf("boom\n");
		scanf("%d",&x);
		if(x==3){	exit(0);	}
		else if(x==2){	writermenu();	}
		else if(x==1){
			memset(abc,'\0',sizeof(abc));
			printf("Enter message : ");
			// fgets(abc,sizeof(abc),stdin);
			scanf("%s",abc);
			// scanf("%[^\n]",abc);
			// printf("abc before strtok : %s\n",abc );
			// strtok(abc,"\n");
			// printf("abc is %s\n",abc);
			// getchar();

			printf("Sending message ...\n");
			memset(res,'\0',sizeof(res));
			sprintf(res,"@%s",abc);
			len = write(my_sock,res,strlen(res));
		}
		else if(x==4){
			memset(abc,'\0',sizeof(abc));
			printf("Enter index : ");
			scanf("%d",&x);
			printf("Enter message : ");
			scanf("%s",abc);
			printf("Sending message ...\n");
			memset(res,'\0',sizeof(res));
			sprintf(res,"%d%s",x,abc);
			len = write(my_sock,res,strlen(res));

		}
		else{
			printf("Invalid input try again\n");
		}
	}
}

void start(){
	int x;
	printf("Choose from the options below :-\n");
	printf("1) Writer\n");
	printf("2) Reader\n");
	if(scanf("%d",&x)<=0){
		printf("Invalid option try agian.\n");
		start();
	}
	if(x>2){
		printf("Invalid option try agian.\n");
		start();	
	}
	if(x==2){
		readermenu();
		reader();
	}
	else{
		writermenu();
		writer();
	}
}
void *recieve_message(void *sock){
	int their_sock = *((int *)sock);
	char msg[500];
	int len;
	while((len = recv(their_sock,msg,500,0)) > 0) {
		msg[len] = '\0';
		fputs(msg,stdout);
		memset(msg,'\0',sizeof(msg));
	}
}
int main(){
	struct sockaddr_in their_addr;
	
	pthread_t sendt,recvt;
	
	char ip[INET_ADDRSTRLEN];
	

	printf("Enter your name : ");
	fgets(username,sizeof(username),stdin);
	strtok(username, "\n");
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(their_addr.sin_zero,'\0',sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//name and socket initialization.


	if(connect(my_sock,(struct sockaddr *)&their_addr,sizeof(their_addr)) < 0) {
		perror("connection not established");
		exit(1);
	}

	strcpy(res,"#");
	strcpy(res,username);
	strcat(res," is now connected to the server");
	// len = write(my_sock,res,strlen(res));

	printf("%s\n",res );
	memset(res,'\0',sizeof(res));
	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf("connected to server\n");
	pthread_create(&recvt,NULL,recieve_message,&my_sock);
	// help_menu();
	start();
	int temp=0;
	pthread_join(recvt,NULL);
	close(my_sock);

}