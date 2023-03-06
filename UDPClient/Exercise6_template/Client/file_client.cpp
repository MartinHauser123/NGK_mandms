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
	printf("Receiving: '%s', size: %li\n", fileName, fileSize);

}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	if (argc < 3){
	    error( "ERROR usage: ""hostname"",  ""filename""\n");
	}




	return 0;
}
