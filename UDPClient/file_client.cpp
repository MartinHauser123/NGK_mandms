/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(const char *);
int main(int argc, char *argv[])
{
   int sock, n, portNr = 9000;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buffer[256];
   
   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
   sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("socket");

   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");

   bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(portNr);
   length=sizeof(struct sockaddr_in);
   
   n=sendto(sock,argv[2],
            strlen(argv[2]),0,(const struct sockaddr *)&server,length);
   if (n < 0) error("Sendto");
   n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
   if (n < 0) error("recvfrom");

   write(1,buffer,n);
   close(sock);
   return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}