#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int readFiles(FILE *key, FILE *plaintext, char **myKey, char **myText){			//checks the length of key generated
	char keyString[2048], textString[2048], ch;
	int i=0, j=0, bufferLength;

 	while (1) {
        ch = fgetc(key);
        if (ch == EOF)	break;
        ++i;
    }

     while (1) {
        ch = fgetc(plaintext);
        if (ch == EOF)	break;
        ++j;
    }
    bufferLength=i+j;


	if(i!=j){
		printf("Keygen length: %d, Plaintext length: %d, lengths must match!!\n", i, j);
		fclose(key);
    	fclose(plaintext);
		exit(1);
	}
	rewind(key);
	rewind(plaintext);

	char *keyTemp=malloc(sizeof(char)*i), *textTemp=malloc(sizeof(char)*j);
	*myKey = malloc(sizeof(char)*i);
	*myText = malloc(sizeof(char)*j);

	i=0;
	j=0;
	while(1){
		ch=fgetc(key);
		if(ch==EOF)	break;
		keyTemp[i]=ch;
		++i;
	}
	while(1){
		ch=fgetc(plaintext);
		if(ch==EOF)	break;
		textTemp[j]=ch;
		j++;
	}
	sprintf(*myKey, keyTemp);
	sprintf(*myText, textTemp);
	free(keyTemp);
	free(textTemp);
	fclose(key);
    fclose(plaintext);
	return bufferLength;
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	//char buffer[256];

	if (argc < 4) { fprintf(stderr,"USAGE: %s port key plaintext\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost");	//host localhost by default
	//serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");



	// Sending data to server (or otp_end_d.c in this case)
	char *keyFile = argv[2];
	char *textFile = argv[3];		//setting filenames
	char *myKey, *myText, *buffer;
	FILE *key = fopen(keyFile, "r");
	FILE *plaintext = fopen(textFile, "r");
	if(key==NULL || plaintext == NULL)	printf("error in opening file...\n");
	int bufferLength = readFiles(key, plaintext, &myKey, &myText);
	//testing
	//printf("mykey is %s, my text is: %s\n", myKey, myText);

	buffer = malloc(sizeof(char)*bufferLength);
	snprintf(buffer, bufferLength, "%s %s", myKey, myText);		//concatenate key and text to buffer
	//testing
	//printf("concatenated string: %s\n", buffer);


	//combineStrings(key, plaintext, bufferLength);

	//printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	//memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	//fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	//buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds



	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}
