/* A simple client in the internet domain using TCP
The ip adresse and port number on server is passed as arguments 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
Extended to support file client!
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include "iknlib.h"

#define STRBUFSIZE 256

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
 * @brief Receives a file from a server socket
 * @param serverSocket Socket stream to server
 * @param fileName Name of file. Might include path on server!
 */

void receiveFile(int serverSocket, const char* fileName, long fileSize)
{
	FILE * fp;
	long bytesToRead = fileSize;
	uint8_t tmpBuffer[1000];
	int n;
	printf("Receiving: '%s', size: %li\n", fileName, fileSize);
	fp = fopen(fileName,"wb");
	while (bytesToRead)
	{
		if (bytesToRead >= 1000)
		{
			n = recv(serverSocket, tmpBuffer, 1000, MSG_WAITALL);
			if (n<0)
				error("someting wong\n");
			fwrite(tmpBuffer,1,1000,fp);
			bytesToRead -= 1000;
			printf("receving 1000 : %li of %li\n", (fileSize-bytesToRead), fileSize);
		}else
		{
			n = recv(serverSocket, tmpBuffer, bytesToRead, MSG_WAITALL);
			if (n<0)
				error("someting wong");
			fwrite(tmpBuffer,1,bytesToRead,fp);
			printf("receving %li : %li of %li\n", bytesToRead, fileSize, fileSize);
			bytesToRead -= bytesToRead;
		}
	}
	fclose(fp);
}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	int sockfd, portno = 9000, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	uint8_t buffer[BUFSIZE];
    
	if (argc < 3)
	    error( "ERROR usage: ""hostname"",  ""filename""\n");

	//portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	    error("ERROR opening socket");

	server = gethostbyname(argv[1]);
	if (server == NULL) 
	    error("ERROR no such host");

	printf("Server at: %s, port: %i\n",argv[1], portno);

	printf("Connect...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	    error("ERROR connecting");


	writeTextTCP(sockfd,argv[2]); // sends filename
		
	
    bzero(buffer,sizeof(buffer));
	long filesize;
	filesize = readFileSizeTCP(sockfd);
	printf("filesize = %li\n", filesize);


	receiveFile(sockfd, argv[2],filesize);

    printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}
