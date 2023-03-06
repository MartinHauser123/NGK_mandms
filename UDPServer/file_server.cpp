/* Creates a datagram server.  The port 
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#define FILENAME_UPTIME "/proc/uptime"
#define FILENAME_LOADAVG "/proc/loadavg"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, char *argv[])
{
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
   while (1) {
       n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
       if (n < 0) error("recvfrom");
	   if (buf[0] == 'u' || buf[0] == 'U')
	   {
			printf("der er modtaget et %s\n", buf);

			FILE * fp;
			char charbuffer[25];
			char ch = 0;
			// open file /proc/uptime
			fp = fopen(FILENAME_UPTIME, "r");
			// load value
			
			//bzero(charbuffer,sizeof(charbuffer));
			for (size_t i = 0; ch != EOF; i++)
			{
				ch = fgetc(fp);
				charbuffer[i] = ch;
				printf("%c",ch);
			}

			printf("\n buffer = %s\n",charbuffer);
			//close file
			fclose(fp);
			// send value
			n = sendto(sock,charbuffer ,sizeof(charbuffer),
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");

	   }
	   else if (buf[0] == 'l' || buf[0] == 'L')
	   {
			printf("der er modtaget et %s\n", buf);

			FILE * fp;
			char charbuffer[30];
			char ch = 0;
			// open file /proc/uptime
			fp = fopen(FILENAME_LOADAVG, "r");
			// load value
			
			//bzero(charbuffer,sizeof(charbuffer));
			for (size_t i = 0; ch != EOF; i++)
			{
				ch = fgetc(fp);
				charbuffer[i] = ch;
				printf("%c",ch);
			}

			printf("\n buffer = %s\n",charbuffer);
			//close file
			fclose(fp);
			// send value
			n = sendto(sock,charbuffer ,sizeof(charbuffer),
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");
	   }
	   else
	   {
			n = sendto(sock,"message not u or L, Got your message\n",37,
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");
			printf("der blev modtaget et forkert tegn: %s\n", buf);
	   }
   }
   return 0;
 }
