/***************************************/
/*              TCP server             */
/***************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
 
#define BUFSIZE 1024                      // buffer size
#define PORT_NO 2001                      // port number
#define error(a,b) fprintf(stderr, a, b)  // error 'function'

int gameOver(int *array1,int *array2){
   for (int i = 0; i < 5; ++i)
   {
      if (array1[i] != 0 || array2[i] != 0)
      {
         return 0;
      }

   }
   return 1;
}

void shuffle(int *array, size_t n) //https://stackoverflow.com/questions/6127503/shuffle-array-in-c
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}
 
int main(int argc, char *argv[] ){  // arg count, arg vector 
 
   /* Declarations */
   int fd;                          // socket endpt
   int fdc[2];                      // socket endpt
   int flags;                       // rcv flags
   struct sockaddr_in server;       // socket name (addr) of server
   struct sockaddr_in client;       // socket name of client
   int server_size;                 // length of the socket addr. server
   int client_size;                 // length of the socket addr. client
   int bytes;                       // length of buffer 
   int rcvsize;                     // received bytes
   int trnmsize;                    // transmitted bytes
   int trnmsize2;
   int err;                         // error code
   char on;                         // 
   char buffer[BUFSIZE+1];          // datagram dat buffer area   
   int pakli[36];                   // pakli
   int card_id;                     //card_id in pakli
   int kez1[5];
   int kez2[5];
   int player1_score;
   int player2_score;

   /* Initialization */
   on                     = 1;
   flags                  = 0;
   bytes                  = BUFSIZE;
   server_size            = sizeof server;
   client_size            = sizeof client;
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port        = htons(PORT_NO);
   card_id = 10, player1_score = 0, player2_score = 0;
   for (int i = 0; i < 36; ++i)
   {
      pakli[i] = 6 + i / 4;
   }
   shuffle(pakli, 36);  
   
   for (int i = 0; i < 10; ++i)  // kezek osztasa
   {
      if (i < 5)
         kez1[i] = pakli[i];
      else
         kez2[i-5] = pakli[i];
   }

   

   /* Creating socket */
   fd = socket(AF_INET, SOCK_STREAM, 0 );
   if (fd < 0) {
      error("%s: Socket creation error\n",argv[0]);
      exit(1);
      }
   
   /* Setting socket options */
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
   setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
   
   /* Binding socket */
   err = bind(fd, (struct sockaddr *) &server, server_size);
   if (err < 0) {
      error("%s: Cannot bind to the socket\n",argv[0]);
      exit(2);
      }
   
   /* Listening */
   err = listen(fd, 10);
   if (err < 0) {
      error("%s: Cannot listen to the socket\n",argv[0]);
      exit(3);
      }
   
   /* Accepting connection request */
   for (int i = 0; i < 2; i++) {
      fdc[i] = accept(fd, (struct sockaddr *) &client, &client_size);
      if (fdc[i] < 0) {
        error("%s: Cannot accept on socket\n",argv[0]);
        exit(4);
      }

      if (i == 0) {
         strcpy(buffer,"Waiting for second player...\n");
         trnmsize = send(fdc[i],buffer,sizeof(buffer),flags);
         if (trnmsize < 0) {
            error("%s: Cannot send data to the client.\n",argv[0]);
            exit(4);
         }
      }

      if(i == 1) {
         strcpy(buffer,"Game is started!\n");
         trnmsize = send(fdc[1],buffer,sizeof(buffer),flags); 
         if (trnmsize < 0) {
              error("%s: Cannot send data to the client.\n",argv[0]);
              exit(4);
         } 
      } 
   }

   // sending cards to clients
   for (int i = 0; i < 2; i++) {
      if(i == 0) {
         trnmsize = send(fdc[i],&kez1,sizeof(kez1),flags);
         if (trnmsize < 0) {
         error("%s: Cannot send data to the client.\n",argv[0]);
         exit(5);
         }
      }
      if (i == 1){
         trnmsize = send(fdc[i],&kez2,sizeof(kez1),flags);
         if (trnmsize < 0) {
         error("%s: Cannot send data to the client.\n",argv[0]);
         exit(5);
         }
      }
   }

   strcpy(buffer,"Your turn!\n");
   send(fdc[0],buffer,sizeof(buffer),flags);
   strcpy(buffer,"Opponents turn!\n");
   send(fdc[1],buffer,sizeof(buffer),flags);
   
   printf("%s\n", "Game is started");

   int last_card = 0;
   int activeP = 1; // 0 = player 1, 1 = player 2
   while(1){
      if (gameOver(kez1, kez2))
      {
         printf("%s\n", "gameove");
         if (player1_score > player2_score)
         {
            strcpy(buffer, "you win");
            send(fdc[0], buffer, sizeof(buffer), flags);
            strcpy(buffer, "you lose");
            send(fdc[1], buffer, sizeof(buffer), flags);
            break;
         }
         else if (player2_score < player1_score)
         {
            strcpy(buffer, "you win");
            send(fdc[1], buffer, sizeof(buffer), flags);
            strcpy(buffer, "you lose");
            send(fdc[0], buffer, sizeof(buffer), flags);
            break;
         }
         else 
         {
            strcpy(buffer, "draw");
            send(fdc[0], buffer, sizeof(buffer), flags);
            send(fdc[1], buffer, sizeof(buffer), flags);
            break;
         }
      }
      strcpy(buffer, "OK, game is not finished");
      send(fdc[0], buffer, sizeof(buffer), flags);
      send(fdc[1], buffer, sizeof(buffer), flags);
      // logic
      
      if (activeP)
      {
         recv( fdc[!activeP], buffer, sizeof(buffer), flags );
         
         if (!strcmp(buffer,"feladom"))
         {
            printf("%s\n", "elso cliens feladta magat");
            strcpy(buffer, "opponent feladta magat, you win!");
            send(fdc[1], buffer, sizeof(buffer), flags);
            strcpy(buffer, "feladtal magat, you lose");
            send(fdc[0], buffer, sizeof(buffer), flags);
            close(fdc[0]);
            close(fdc[1]);
            close(fd);
            exit(0);
         }
         if (!strcmp(buffer,"1"))
         {
            char str[12];
            last_card = kez1[0];
            sprintf(str, "%d", kez1[0]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez1[0] = pakli[card_id++];
            else
               kez1[0] = 0;
         }

         if (!strcmp(buffer,"2"))
         {
            char str[12];
            last_card = kez1[1];
            sprintf(str, "%d", kez1[1]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez1[1] = pakli[card_id++];
            else
               kez1[1] = 0;
         }

         if (!strcmp(buffer,"3"))
         {
            char str[12];
            last_card = kez1[2];
            sprintf(str, "%d", kez1[2]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez1[2] = pakli[card_id++];
            else
               kez1[2] = 0;
         }

         if (!strcmp(buffer,"4"))
         {
            char str[12];
            last_card = kez1[3];
            sprintf(str, "%d", kez1[3]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez1[3] = pakli[card_id++];
            else
               kez1[3] = 0;
         }

         if (!strcmp(buffer,"5"))
         {
            char str[12];
            last_card = kez1[4];
            sprintf(str, "%d", kez1[4]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez1[4] = pakli[card_id++];
            else
               kez1[4] = 0;
         }

         
         send(fdc[1], buffer, sizeof(buffer), flags);
         send(fdc[0],&kez1,sizeof(kez1),flags);
      }
      else if (!activeP)
      {
         recv( fdc[!activeP], buffer, sizeof(buffer), flags);
         
         if (!strcmp(buffer,"feladom"))
         {
            printf("%s\n", "masodik cliens feladta magat");
            strcpy(buffer, "opponent feladta magat, you win!");
            send(fdc[0], buffer, sizeof(buffer), flags);
            strcpy(buffer, "feladtal magat, you lose");
            send(fdc[1], buffer, sizeof(buffer), flags);
            close(fdc[0]);
            close(fdc[1]);
            close(fd);
            exit(0);
         }

         if (!strcmp(buffer,"1"))
         {
            char str[12];
            if (kez2[0] == 10 || kez2[0] == 14)
            {
               player2_score += 10;
            }
            else if (last_card > kez2[0])
            {
               player2_score += 10;
            }
            else
            {
               player1_score += 10;
            }
            sprintf(str, "%d", kez2[0]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez2[0] = pakli[card_id++];
            else
               kez2[0] = 0;
         }

         if (!strcmp(buffer,"2"))
         {
            char str[12];
            if (kez2[1] == 10 || kez2[1] == 14)
            {
               player2_score += 10;
            }
            else if (last_card > kez2[1])
            {
               player2_score += 10;
            }
            else
            {
               player1_score += 10;
            }
            sprintf(str, "%d", kez2[1]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez2[1] = pakli[card_id++];
            else
               kez2[1] = 0;
            
         }

         if (!strcmp(buffer,"3"))
         {
            char str[12];
            if (kez2[2] == 10 || kez2[2] == 14)
            {
               player2_score += 10;
            }
            else if (last_card > kez2[2])
            {
               player2_score += 10;
            }
            else
            {
               player1_score += 10;
            }
            sprintf(str, "%d", kez2[2]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez2[2] = pakli[card_id++];
            else
               kez2[2] = 0;
         }

         if (!strcmp(buffer,"4"))
         {
            char str[12];
            if (kez2[3] == 10 || kez2[3] == 14)
            {
               player2_score += 10;
            }
            else if (last_card > kez2[3])
            {
               player2_score += 10;
            }
            else
            {
               player1_score += 10;
            }
            sprintf(str, "%d", kez2[3]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez2[3] = pakli[card_id++];
            else
               kez2[3] = 0;
         }

         if (!strcmp(buffer,"5"))
         {
            char str[12];
            if (kez2[4] == 10 || kez2[4] == 14)
            {
               player2_score += 10;
            }
            else if (last_card > kez2[4])
            {
               player2_score += 10;
            }
            else
            {
               player1_score += 10;
            }
            sprintf(str, "%d", kez2[4]);
            strcpy(buffer, str);
            if (card_id < 36)
               kez2[4] = pakli[card_id++];
            else
               kez2[4] = 0;
         }

         send(fdc[0], buffer, sizeof(buffer), flags);
         send(fdc[1], &kez2, sizeof(kez2), flags);
      }
      
      
      activeP = !activeP;
   }

   recv(fdc[0], buffer, sizeof(buffer), flags);
   if (!strcmp(buffer, "vege"))
   {
      close(fdc[0]);
      close(fdc[1]);
      close(fd);
      exit(0);
   }
   if (!strcmp(buffer, "ujra"))
   {
      printf("%s\n", "not implemented, restart program if u want");
   }
   recv(fdc[1], buffer, sizeof(buffer), flags);
   if (!strcmp(buffer, "vege"))
   {
      close(fdc[0]);
      close(fdc[1]);
      close(fd);
      exit(0);
   }
   if (!strcmp(buffer, "ujra"))
   {
      printf("%s\n", "not implemented, restart program if u want");
   }

   /* Closing sockets and quit */
   close(fdc[0]);
   close(fdc[1]);
   close(fd);
   exit(0);
 
}




































