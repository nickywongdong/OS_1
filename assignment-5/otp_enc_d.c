#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//splits the string received from client to be encrypted
void parseBuffer(char buffer[2048], char **key, char **text){
	char *keyTemp, *textTemp, *temp;
	int i, count;

  	temp = strtok (buffer,"\n");	//tokenize the buffer twice (only should be two separate strings)
  	//printf("length of temp: %d\n", strlen(temp));
  	*key = malloc(sizeof(char)*strlen(temp)+1);		//need the additional 1 '\0'
  	*text = malloc(sizeof(char)*strlen(temp)+1);		//we will assume that both strings are same length
  	snprintf(*key, strlen(temp)+1, temp);
  	temp = strtok(NULL, "\n");
  	snprintf(*text, strlen(temp)+1, temp);
}

//encrypt text based off key
void encryptMessage(char *key, char *text){
	char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int *temp1 = malloc(sizeof(int)*strlen(text)+1);
	int *temp2 = malloc(sizeof(int)*strlen(key)+1);	//same length, but for sake of formality
	int *temp3 = malloc(sizeof(int)*strlen(text)+1);	//to store the encrypted msg
	int i, j;

	//testing
	printf("strlen of text: %d\n", strlen(text)+1);

	//store text as its integer value (from alpha array) into temp1
	for(i=0; i<strlen(text)+1; i++){
		for(j=0; j<27; j++){
			if(text[i]==alpha[j])	temp1[i]=j;
		}
	}
	//store key as its integer value (from alpha array) into temp2
	for(i=0; i<strlen(key)+1; i++){
		for(j=0; j<27; j++){
			if(key[i]==alpha[j])	temp2[i]=j;
		}
	}

	//testing
	printf("temp1: \n");
	for(i=0; i<37; i++)	printf("%d ", temp1[i]);
	printf("\ntemp2: \n");
	for(i=0; i<37; i++)	printf("%d ", temp2[i]);
	printf("\n");

	printf("last temp1: %d\n", temp1[36]);
	printf("last temp2: %d\n", temp2[36]);



	//add the two values together
	for(i=0; i<strlen(text)+1; i++){
		//printf("temp1 at %d: %d\n", i, temp1[i]);
		//printf("temp2 at %d: %d\n", i, temp2[i]);
		temp3[i]=temp1[i]+temp2[i];
		//printf("temp3 at %d (should be sum): %d\n", i, temp3[i]);
		if(temp3[i]>27)	temp3[i]=temp3[i]-27;	//make sure our # doesn't go over 27
	}

	//testing
	//for(i=0; i<27; i++)	printf("temp3 at %d: %d\n", i, temp3[i]);
	//printf("text: %s\n", text);
	//printf("temp: %s\n", temp1);
	//printf("temp at 0: %d\n", temp1[0]);
	//printf("temp at 1: %d\n", temp1[1]);
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[2048];		//arbitrary length
	char *key, *text;
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	// Get the message from the client and display it
	memset(buffer, '\0', 2048);
	charsRead = recv(establishedConnectionFD, buffer, 2048, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

	parseBuffer(buffer, &key, &text);
	encryptMessage(key, text);

	// Send a Success message back to the client
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0;
}
