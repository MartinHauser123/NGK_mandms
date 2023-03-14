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

void SendFileContent(int sock, sockaddr_in *from, socklen_t *fromlen, const char* filename)
{
	FILE * fp;
	char charbuffer[35];
	char ch = 0;
	int n, bufLen;
	// open file /proc/uptime
	fp = fopen(filename, "r");
	// load value
	bzero(charbuffer, sizeof(charbuffer));
	//bzero(charbuffer,sizeof(charbuffer));
	for (int i = 0; ch != EOF; i++)
	{
		ch = fgetc(fp);
		charbuffer[i] = ch;
		bufLen = i;
	}
	printf("\n buffer = %s\n",charbuffer);
	//close file
	fclose(fp);
	// send value
	n = sendto(sock,charbuffer , bufLen,
			0,(struct sockaddr *)from,*fromlen);
	if (n  < 0) error("sendto");
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
	   printf("Der er modtaget et %s\n", buf);
	   if (buf[0] == 'u' || buf[0] == 'U')
			SendFileContent(sock, &from, &fromlen, FILENAME_UPTIME);
	   else if (buf[0] == 'l' || buf[0] == 'L')
			SendFileContent(sock, &from, &fromlen, FILENAME_LOADAVG);
	   else
	   {
			n = sendto(sock,"message not u or L, Got your message\n",37,
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");
			printf("Der blev modtaget et forkert tegn: %s\n", buf);
	   }
   }
   return 0;
 }
