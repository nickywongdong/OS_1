/*Nick Wong
  Thursday, October 27th
  OS-Class assign2
  wongnich.adventure.c
  */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>


typedef int bool; //for true & false
#define true 1
#define false 0

//for our thread and mutexes
pthread_mutex_t lock;
pthread_t thread0;

struct Room{
   char roomName[20];
   char roomConnections[6][50]; //at most 6 room connections
   int roomType; //starting room... ending room... etc. (0:start room, 1:mid room, 3: end room)
   int numConnections;
   int maxConnections;
};

struct Room roomsList[10];    //10 rooms in structs, global because it's used eveywhere

void initRooms(struct Room roomsList[10]);
void shuffleRooms(struct Room roomsList[10]);
void AddRandomConnection();
bool IsGraphFull();
void mainGameLoop(char dirPath[7][50]);


void* timeGet(){    //gets the time, and places it in a file named "time"

   pthread_mutex_lock(&lock);    //lock the thread

   char dirName[50], dirPath[50];
   sprintf(dirName, "wongnich.rooms.%d", getpid()); //adding process-id to dirname
   memset(dirPath, '\0', 50);
   sprintf(dirPath, "%s/time", dirName);
   FILE *fp = fopen(dirPath, "w");
   time_t t;
   time(&t);
   fprintf(fp, "%s", ctime(&t));
   fclose(fp);

   pthread_mutex_unlock(&lock);  //we're done, unlock the thread
}

int main(){

   int i, j, ret;
   srand(time(NULL));

   char dirName[20];
   memset(dirName, '\0', 50);
   initRooms(roomsList);  //hardcode 10 rooms
   shuffleRooms(roomsList);   //"shuffle" our rooms so they are random each load

   sprintf(dirName, "wongnich.rooms.%d", getpid()); //adding process-id to dirname

   mkdir(dirName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creating directory for files

   char tempRoom[10][50];
   for(i=0; i<10; i++){
      memset(tempRoom[i], '\0', 50);
      strcpy(tempRoom[i], roomsList[i].roomName);
   }

   //creating file paths:
   char dirPath[7][50];

   for(i=0; i<7; i++){
      memset(dirPath[i], '\0', 50);
      strcpy(dirPath[i], dirName);
      sprintf(dirPath[i], "%s/%s", dirPath[i], roomsList[i].roomName);
   }

   FILE *files[7];       //randomly (because we "shuffled") creates 7 room files
   for(i = 0; i < 7; i++){
      files[i] = fopen(dirPath[i], "w");
   }


   // Create all connections in graph
   while (IsGraphFull() == false)
   {
      AddRandomConnection();
   }

   //write the connections to the files:
   for (i = 0; i < 7; i++) {
      fprintf(files[i], "ROOM NAME: %s\n", roomsList[i].roomName);

      // For each room, write the list of connected rooms
      int j;
      for (j = 0; j < roomsList[i].maxConnections; j++) {
	 fprintf(files[i], "CONNECTION %d: %s\n", j+1, roomsList[i].roomConnections[j]);
      }
      //define the type of room
      if (roomsList[i].roomType == 1) {
	 fprintf(files[i], "ROOM TYPE: %s\n", "START_ROOM");
      }
      else if (roomsList[i].roomType == 2) {
	 fprintf(files[i], "ROOM TYPE: %s\n", "MID_ROOM");
      }
      else if (roomsList[i].roomType == 3) {
	 fprintf(files[i], "ROOM TYPE: %s\n", "END_ROOM");
      }
      //finished
      fclose(files[i]);
   }

   //now we run the game:
   mainGameLoop(dirPath);

   //timeGet(dirName);
   return 0;
}


int locateCurrentRoom(char dirPath[7][50], char searchString[512]){ //will always locate the current room (returns the iterator)
   int i, currentRoom;
   char temp[512];

   FILE *files[7];
   for(i=0; i<7; i++){
      files[i] = fopen(dirPath[i], "r");
   }
   //let's search for the specified room
   for(i=0; i<7; i++){
      while(fgets(temp, 512, files[i]) != NULL) {
	 if((strstr(temp, searchString)) != NULL) {
	    currentRoom=i;
	    break;
	 }
      }
   }
   for(i=0; i<7; i++)  fclose(files[i]);
   return currentRoom;

}

void listPossibleConnections(int currentRoom, char dirPath[7][50]){   //will print out all possible locations
   int i, iter=1;
   char temp[512];
   char possible[512];
   char junk[512];

   FILE *fp;
   printf("POSSIBLE CONNECTIONS: ");
   fp = fopen(dirPath[currentRoom], "r");
   for(i=0; i<roomsList[currentRoom].maxConnections; i++){
      while(fgets(temp, 512, fp)!=NULL){
	 if(strstr(temp, "CONNECTION") != NULL){
	    sscanf (temp,"%s %s %s", junk, junk, possible);
	    printf ("%s, ",possible);
	 }
      }
   }

   fclose(fp);
}

bool checkInput(int currentRoom, char input[512]){       //will check if that location is possible and return true/false
   int i, j;

   for(i=0; i<roomsList[currentRoom].maxConnections; i++){
      if(strcmp(roomsList[currentRoom].roomConnections[i], input)==0){    
	 return true;
      }
   }
   if(strcmp(input, "time")==0)  return true;
   else{
      printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
      return false;
   }
}

void displayTime(){   //reads time from the file we created
   FILE *fp;
   int c;
   char dirName[50], dirPath[50];
   sprintf(dirName, "wongnich.rooms.%d", getpid());
   sprintf(dirPath, "%s/time", dirName);
   fp = fopen(dirPath, "r");
   while ((c = getc(fp)) != EOF)
      printf("%c", c);
   fclose(fp);
}

void mainGameLoop(char dirPath[7][50]){   //where our game will primarily loop
   int i, j, currentRoom, endRoom, steps=0; 
   struct Room A, B;
   char temp[512], input[512], possible[512], tempInput[512], path[50][50];

   //let's search for the start/end room
   currentRoom=locateCurrentRoom(dirPath, "START_ROOM");
   endRoom=locateCurrentRoom(dirPath, "END_ROOM");

   while(currentRoom!=endRoom){
      //continue asking until we receive "good" input
      do{
	 printf("\nCURRENT LOCATION: %s\n", roomsList[currentRoom].roomName);
	 listPossibleConnections(currentRoom, dirPath);
	 printf("\nWHERE TO? >");
	 scanf("%s", &input);
	 //mutex
	 if(strcmp(input, "time")==0){
	    pthread_create(&thread0, NULL, timeGet, NULL);
	    pthread_join(thread0, NULL);
	    displayTime();
	 }
      } while(checkInput(currentRoom, input)==false);
      sprintf(tempInput, "ROOM NAME: %s", input);
      strcpy(path[steps], input);
      currentRoom=locateCurrentRoom(dirPath, tempInput);    //update currentRoom
      steps++;    //if successful input, increment number of steps
   }

   //we're finished, display end results:
   printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
   for(i=0; i<steps; i++)  printf("%s\n", path[i]);
}


bool IsGraphFull() // Returns true if all rooms have 3 to 6 outbound connections, false otherwise
{
   int i;
   for(i=0; i<7; i++){
      if(roomsList[i].numConnections<roomsList[i].maxConnections) //if room doesn't have max connections, keep going
	 return false;
   }
   return true;
}

bool hasRoomConnected(struct Room x, struct Room y){    //returns true if the room already has that room connected
   int i;
   //if the array of room consists of the room y, then return true
   for(i=0; i<x.maxConnections; i++){
      if(strcmp(x.roomConnections[i], y.roomName)==0)
	 return true;
   }
   return false;
}

//connect the two rooms
void connectRooms(int x, int y){
   strcpy(roomsList[x].roomConnections[roomsList[x].numConnections], roomsList[y].roomName);
   roomsList[x].numConnections++;
   strcpy(roomsList[y].roomConnections[roomsList[y].numConnections], roomsList[x].roomName);
   roomsList[y].numConnections++;
}

void AddRandomConnection(){ // Adds a random, valid outbound connection from a Room to another Room
   //struct Room A, B;
   int i, A, B;
   while(true){
      A=rand()%7;   //rand num from 0-6
      if(roomsList[A].numConnections<roomsList[A].maxConnections) break;
   }
   //keep finding a new room to connect if they are: the same room, or they have connected once already
   while(A==B || hasRoomConnected(roomsList[A], roomsList[B])==true){
      B=rand()%7;   //Pick rand room different from A
   }
   connectRooms(A, B);

}


void initRooms(struct Room roomsList[10]){     //sets up our rooms, just hard coded the names in

   int i, j;
   for(i=0; i<10; i++){
      memset(roomsList[i].roomName, '\0', 50);         //init roomNames
      roomsList[i].numConnections = 0; //each room starts with 0 connections
      roomsList[i].roomType=2;         //make each room a mid type to begin with
      roomsList[i].maxConnections=rand() % 4 + 3;  //should be random number 3-6
      for(j=0; j<6; j++){
	 memset(roomsList[i].roomConnections[j], '\0', 50);  //init roomConnections array
      }

   }
   strcpy(roomsList[0].roomName, "BASEMENT");
   strcpy(roomsList[1].roomName, "LIBRARY");
   strcpy(roomsList[2].roomName, "CLASSROOM");
   strcpy(roomsList[3].roomName, "BATHROOM");
   strcpy(roomsList[4].roomName, "GYM");
   strcpy(roomsList[5].roomName, "GARDEN");
   strcpy(roomsList[6].roomName, "ATTIC");
   strcpy(roomsList[7].roomName, "MAUSOLEUM");
   strcpy(roomsList[8].roomName, "OFFICE");
   strcpy(roomsList[9].roomName, "ATRIUM");

   //Select a start room, and end room
   roomsList[0].roomType = 1;    //since we will be shuffling, it doesn't matter that we hardcode this
   roomsList[1].roomType = 3;

   return;
}

void shuffleRooms(struct Room roomsList[10]){      //"randomizes our rooms array"

   int i, x, randNum;
   char temp[10];
   memset(temp, '\0', 10);

   for(x=0; x<100; x++){
      for(i=0; i<9; i++){
	 randNum=rand() % 10;
	 strcpy(temp, roomsList[i].roomName);
	 strcpy(roomsList[i].roomName, roomsList[randNum].roomName);
	 strcpy(roomsList[randNum].roomName, temp);
      }
   }



   return;
}
