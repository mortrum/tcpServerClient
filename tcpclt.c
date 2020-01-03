/***************************************/
/*             TCP client              */
/***************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
 
#define BUFSIZE 1024
#define PORT_NO 2001
#define error(a,b) fprintf(stderr, a, b)

void printCards(int *kez){
	for (int i = 0; i < 5; ++i){
   	printf("%d ", kez[i]);
   }
   printf("\n");
}
 
int main(int argc, char *argv[] ) {// arg count, arg vector   
	
	/* Declarations */
   int fd;                         // socket endpt  
   int flags;                      // rcv flags
   struct sockaddr_in server;        // socket name (addr) of server 
   struct sockaddr_in client;        // socket name of client 
   int server_size;                // length of the socket addr. server 
   int client_size;                // length of the socket addr. client 
   int bytes;                        // length of buffer 
   int rcvsize;                    // received bytes
   int trnmsize;                   // transmitted bytes
   int err;                        // error code
   int ip;                                               // ip address
   char on;                        // 
   char buffer[BUFSIZE+1];         // datagram dat buffer area
   char server_addr[16];           // server address    
 	int my_turn;

   /* Initialization */
   on    = 1;
   flags = 0;
   my_turn = 0;
   server_size = sizeof server;
   client_size = sizeof client;
   sprintf(server_addr, "%s", "127.0.0.1"); //argv[1]
   ip = inet_addr(server_addr);
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = ip;
   server.sin_port        = htons(PORT_NO);
    
   /* Creating socket */
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if (fd < 0) {
      error("%s: Socket creation error.\n",argv[0]);
      exit(1);
   }
 
   /* Setting socket options */
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
   setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
 
   /* Connecting to the server */
   err = connect(fd, (struct sockaddr *) &server, server_size);
   if (err < 0) {
      error("%s: Cannot connect to the server.\n", argv[0]);
      exit(2);
   }

   rcvsize = recv( fd, buffer, sizeof(buffer), flags );
   if (rcvsize < 0) {
      error("%s: Cannot receive data from the socket.\n", argv[0]);
      exit(4);
   }
   printf("%s", buffer);

   int kez[5];
   rcvsize = recv( fd, &kez, sizeof(kez), flags );
   if (rcvsize < 0) {
      error("%s: Cannot receive data from the socket.\n", argv[0]);
      exit(5);
   }

   printf("%s", "My cards: ");
   for (int i = 0; i < 5; ++i)
   {
   	printf("%d ", kez[i]);
   }
   printf("\n");

   rcvsize = recv( fd, buffer, sizeof(buffer), flags );
   if (rcvsize < 0) {
      error("%s: Cannot receive data from the socket.\n", argv[0]);
      exit(5);
   }
   printf("%s", buffer);

   if (!strcmp(buffer, "Your turn!\n"))
   	my_turn = 1;
   else
   	my_turn = 0;


   while(1){
   	if (my_turn)
   	{
   		//checking game ending
   		recv(fd, buffer, sizeof(buffer), flags);
   		if (!strcmp(buffer, "you win")){
   			printf("%s\n", "you win!! gratz");
   			break;
   		}
   		else if (!strcmp(buffer, "you lose")){
   			printf("%s\n", "you lose!! next time..");
   			break;
   		}
   		else if (!strcmp(buffer, "draw")){
   			printf("%s\n", "draw!!");
   			break;
   		}
   		else if (!strcmp(buffer, "opponent feladta magat, you win!"))
   		{
   			printf("%s\n", "opponent feladta magat, you win!!");
   			close(fd);
   			exit(0);
   			break;
   		}
   		else if (!strcmp(buffer, "feladtal magat, you lose"))
   		{
   			printf("%s\n", "feladtal magat, you lose!!");
   			close(fd);
   			exit(0);
   			break;
   		}



   		//game
   		char move[20];
   		printf("%s\n", "Selet your move");
   		printf("%s", "My cards: ");
   		printCards(kez);

   		scanf("%s", move);
   		if (!strcmp(move, "feladom"))
   		{
   			send(fd,move,sizeof(move),flags);
   			close(fd);
   			exit(0);
   		}
   		send(fd,move,sizeof(move),flags);
   		recv(fd, &kez, sizeof(kez), flags);
   	}
   	else if (!my_turn)
   	{
   		//checking game ending
   		recv(fd, buffer, sizeof(buffer), flags);
   		if (!strcmp(buffer, "you win")){
   			printf("%s\n", "you win!! gratz");
   			break;
   		}
   		else if (!strcmp(buffer, "you lose")){
   			printf("%s\n", "you lose!! next time..");
   			break;
   		}
   		else if (!strcmp(buffer, "draw")){
   			printf("%s\n", "draw!!");
   			break;
   		}
   		else if (!strcmp(buffer, "opponent feladta magat, you win!"))
   		{
   			printf("%s\n", "opponent feladta magat, you win!!");
   			close(fd);
   			exit(0);
   			break;
   		}
   		else if (!strcmp(buffer, "feladtal magat, you lose"))
   		{
   			printf("%s\n", "feladtal magat, you lose!!");
   			close(fd);
   			exit(0);
   			break;
   		}


   		//game
   		recv(fd, buffer, sizeof(buffer), flags );
   		printf("Your opponent move is: %s\n", buffer);
   		
   	}
   	else
   		printf("%s\n", "hiba");

   	
   	my_turn = !my_turn;
   	
   }

   printf("%s\n", "Do you want return match or end sesion?");
   char result[20];
   scanf("%s", result);
   send(fd,result,sizeof(result),flags);
   /* ujra not implemented
   */
   /* Closing sockets and quit */
   close(fd);
   exit(0);
} 








