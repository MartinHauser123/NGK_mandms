// File handling - the C way
// Michael Alrøe
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFSIZE 20

//#define ONEBYTE
//#define THREEBYTES

int main ()
{
	char fileName[]	= "test.bin";
	uint8_t Buffer[BUFSIZE];
	size_t numBytes;
	FILE * fp;

	// Generate test data
	for (int i=0; i<BUFSIZE; i++) {
		Buffer[i]=i;
	}
	for (int i=0; i<BUFSIZE; i++) printf("%i ", Buffer[i]);
	printf("\n");
	
    //*** Write file demo ***
	fp = fopen(fileName, "wb");   // Open file handle, write binary
	fwrite(Buffer, 1, sizeof(Buffer), fp);
	fclose(fp);  // Close file handle
	
    bzero(Buffer, BUFSIZE);  // Not needed - just for test!!!
	
    //*** Read file demo ***
	fp = fopen(fileName, "rb");   // Open file handle, read binary
	#ifdef ONEBYTE
		printf("One byte buffer\n");
		uint8_t tmpBuf;
		numBytes = fread(&tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
		while (numBytes) {
			printf("%i ", tmpBuf);
			numBytes = fread(&tmpBuf, 1, 1, fp);  // Automatic seek!
		}
		printf("\r\n");
	#elif defined (THREEBYTES)
		#define TMPBUFSIZE 3
		uint8_t tmpBuf[TMPBUFSIZE];
		printf("Three byte buffer\n");
		numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
		while (numBytes) {
			for (int j=0; j<numBytes; j++)
			{
				printf("%i ", tmpBuf[j]);
			}
		printf("\r\n");
		numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
		}

	#else
		uint8_t tmpBuf[BUFSIZE];
		numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
		for (int i=0; i<numBytes; i++) 
		{
			printf("%i ", tmpBuf[i]);
		}
		printf("\r\n");

	#endif

	fclose(fp);  // Close file handle
	
	return (0);
}