/* A simple server in the internet domain using TCP
The port number is passed as an argument 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
Extended to support file server!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

#define STRBUFSIZE 256

#define BUFSIZE_RX 200
#define BUFSIZE_TX 256

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
 * @brief Sends a file to a client socket
 * @param clientSocket Socket stream to client
 * @param fileName Name of file to be sent to client
 * @param fileSize Size of file
 */
void sendFile(int clientSocket, const char* fileName, long fileSize)
{
	FILE * fp;
	size_t numBytes;

	printf("Sending: %s, size: %li\n", fileName, fileSize);
    
	fp = fopen(fileName, "rb");   // Open file handle, read binary

	uint8_t tmpBuf[1000];
	long prog = 0;
	numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
	while (numBytes) {
		// for (int j=0; j<numBytes; j++)
		// {
		// 	printf("%i ", tmpBuf[j]);
		// }
		write(clientSocket, tmpBuf, numBytes);
		prog += numBytes;
		printf("sending %li : %li off %li",numBytes,prog,fileSize);
		printf("\r\n");
		numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
	}
}


int main(int argc, char *argv[])
{
	printf("Starting server...\n");

	//connect tcp
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	char filename[100];
	

	if (argc < 2) 
		error("ERROR usage: port");


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	printf("Binding...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	printf("Listen...\n");
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr);

	for (;;)
	{
		
		printf("Accept...\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) error("ERROR on accept");
		else printf("Accepted\n");
		
		bzero(filename,sizeof(filename));

		readTextTCP(newsockfd, filename, sizeof(filename));
		
		long filesize = getFilesize(filename);
		if(filesize < 1)
		{
			printf("fil ikke fundet\n");
		}else
		{
			printf("fil fundet size = %li\n", filesize);
		}
		char filesizeChar[100];
		sprintf(filesizeChar,"%li",filesize);
		writeTextTCP(newsockfd, filesizeChar);
		if (filesize>0)
		{
			sendFile(newsockfd, filename, filesize);
		}else
		{
			printf("filen findes ikke\n");
		}
		

		close(newsockfd);
	}
	close(sockfd);
	return 0;  
}