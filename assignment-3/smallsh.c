/*Nick Wong
 Tuesday, November 15th
 OS-Class assign3
 smallsh.c
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

bool isBackground=false;
bool isOutput=false;
bool isInput=false;

bool SHELL=true;

char inFile[50];
char outFile[50];

int status=-5;    //overall status    (can change from global later if we run into problems)
int displayedStatus=0;  //status that we print out (different from the status due to while loop in main)

pid_t spawnpid=-5;

/*

  store process ID's in null-terminator filled array. We can check to see which process have been exited. Then "bubble sort", null terminators to the end
  upon exiting shell, we will check this array to close any processes that are still open
*/

//simply exits the shell, will set status to 0 (breaking out of loop)
void execExit(){
  SHELL=false;  //return;
  //exit(1);
}

//runs cd build in command
void execCd(char *cmd, char*args[]){
  //testing
  //printf("IN CD\n");
  char *home = getenv("HOME");
  if(args[1][0]=='\0'){
    chdir(home);
  }
  else if(!chdir(args[1])){
    printf("error in changing directories...\n");
  }
}

//will display status of prev commands
void execStatus(){
  printf("Status: %d\n", displayedStatus);
  displayedStatus=0;
}

//will execute all other commands based off PATH variable: /usr/built-in
void execOther(char *line, char *cmd, char *args[], int x){
  int i;

  spawnpid = fork();
  if(spawnpid==0){  //child
    signal (SIGINT, SIG_DFL);    //reset signal to normal behavior

    int fd;
    //if our flag was set to output

    //check if we want to run in background
    if(isBackground){
      int devNull = open("/dev/null", O_WRONLY);
      dup2(devNull, 1);   //redirects to NULL, no error handling (will simply run without outputting);
      close(devNull);
    }

    if(isOutput){
      fd=open(outFile, O_RDONLY, 0777);   //open for reading
      if (fd == -1) {
        printf("The file could not be opened for redirection...\n");
        fflush(stdout);
        displayedStatus=-1;
        exit(1);
      }
      else{
        if(dup2(fd, 0)==-1){    //redirect from stdout (0) to filedecriptor
          printf("cannot redirect stdin...\n");
          fflush(stdout);
        }
      }
      displayedStatus=errno;
      close(fd);

    }
    //if our flag was set to input
    if(isInput){
      fd=open(inFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);    //open for writing (give all permission doesn't really matter)
      //error handling like a good kid
      if (fd == -1) {
        printf("The file could not be opened for redirection...\n");
        fflush(stdout);
        displayedStatus=-1;
        exit(1);
      }
      else{
        //printf("file descriptor %d\n", fd);
        if(dup2(fd, 1)==-1){        //redirect from filedescriptor
          printf("cannot redirect stdout...\n");
          fflush(stdout);
        }
        displayedStatus=errno;
        close(fd);
      }
    }

    for(i=x; i<512; i++){   //this sets the last arg to NULL so that execvp knows when to stop
      //free(args[i]);
      args[i] = NULL;       //should not affect main shell because it's inside fork()
      //printf("arg %d: %s\n", i, args[i]);
    }
    //printf("command: %s\n", cmd);
    execvp(cmd, args);    //simply run the cmd with the arguments
    //if child returns from execvp, then there was an error
    printf("error in executing command...\n");
    exit(1);
    displayedStatus=-1;
  }
  else if(spawnpid<0){    //ran into an error
    printf("ERROR\n");
    status=-1;
  }
  else{               //it's the parent
    if(isBackground){ //if we saw an &
      printf("PID of Background Process: %d\n", spawnpid);
      fflush(stdout);
      int background=waitpid(0, &displayedStatus, WNOHANG);   //parent will not wait for child to terminate
      //printf("value of background: %d\n", background);
      if(background==-1)  printf("error has ocurred...\n");
      //else  if(WIFEXITED(displayedStatus))  printf("process %d has terminated\n", spawnpid);
    }
    //this will search for any running processes
    //waitpid(0, &displayedStatus, WNOHANG);   //parent will not wait for child to terminate

    else  {
      waitpid(spawnpid, &displayedStatus, 0);     //parent wait for child to terminate
      if(WIFEXITED(displayedStatus))  printf("process %d has terminated\n", spawnpid);
  }
}
}
//will execute the commands based off of cmd string (checks for built-in commands first)
void execCommands(char *line, char*cmd, char*args[], int x){
  //first we check for built-in commands
  //testing
  if(strcmp(args[0], "exit")==0)  execExit();
  else if(strcmp(args[0], "cd")==0) execCd(cmd, args);
  else if(strcmp(args[0], "status")==0) execStatus();
  else if(!strcmp(args[0], "(null)") || args[0][0]=='#') return;   //this means arg was null, or comment
  else  execOther(line, cmd, args, x);
}
//splits the input string by spaces, and stops at the newline
int parseInput(char **line, char **cmd, char *args[]){
  int i, x=0;
  memset(*cmd, '\0', 256);    //make sure cmd is properly init, or we reset cmd from last time
  char temp[256];   //to store strtok temporarily
  memset(temp, '\0', 256);

  snprintf(args[0], 50, "%s", strtok(*line,"\n ,-"));    //initial strtok
  //printf("arg 0: %s\n", args[0]);
  strcpy(temp, args[0]);
  x++;
  while(1){  //will copy in string "(null)" (dont know why)
    //testing
    //snprintf(args[x], 50, "%s", strtok(NULL,"\n ,-"));

    snprintf(temp, 50, "%s", strtok(NULL,"\n ,-"));     //first copy strtok into a temp to compare
    if(strcmp(temp, "(null)")==0)  break;     //
    if(temp[0]=='<') {
      isOutput=true;
      snprintf(temp, 50, "%s", strtok(NULL,"\n ,-"));
      strcpy(outFile, temp);    //then end, we shouldn't copy redirection to arguments

      //snprintf(args[x], 50, "%s", temp);      //copy the temp into args
    }

    else if(temp[0]=='>'){
      isInput=true;
      snprintf(temp, 50, "%s", strtok(NULL,"\n ,-"));
      strcpy(inFile, temp);   //same as above
    }
    else if(temp[0]=='&'){
      isBackground=true;
    }

    else{   //if it's none of the redirections, then it should be an argument
      strcpy(args[x], temp);
       x++;
    }
  }
  //store first arg as cmd
  strcpy(*cmd, args[0]);            //this is broken
  int temp_x=x;
  /*if(args[x][0]=='<'){
    isOutput=true;
    strcpy(outFile, args[x+1]);
    //printf("outfile: %s\n", outFile);
  }
  else if(args[x][0]=='>'){
    isInput=true;
    strcpy(inFile, args[x+1]);
    //printf("infile: %s\n", inFile);
  }
  else if(args[x][0]=='&'){
    isBackground=true;
  }*/

  //we then return that initial x value so we can reallocate memory for what we free'ed
  return temp_x;
}


//simply reads in input as a string, and stores in line
void readInput(char **line){
  size_t bufsize = 0;
  memset(*line, '\0', 2048);    //reset line
  getline(&*line, &bufsize, stdin);
  fflush(stdin);

  //testing
  //printf("input: %s\n", line);
}
void smallsh(){
  int i, x;
  signal(SIGINT, SIG_IGN);
  char *line=malloc(sizeof(char)*2048), *cmd=malloc(sizeof(char)*256);
  char *args[512];
  for(i=0; i<512; i++)  args[i]=malloc(sizeof(char)*256);

  while(SHELL){
    //if(WIFEXITED(displayedStatus))  printf("process %d has terminated\n", spawnpid);
    isBackground=false;
    isOutput=false;
    isInput=false;
    printf(": ");
    fflush(stdout);
    readInput(&line);
    //testing
    //printf("status: %d\n", status);
    //printf("line: %s\n", line);
    //for(i=0; i<512; i++)
    //  printf("arg %d: %s\n", i, args[i]);
    x=parseInput(&line, &cmd, args);
    //for(i=0; i<512; i++){
    //  if(args[i]!=NULL) printf("arg %d: %s\n", i, args[i]);
    //}
    execCommands(line, cmd, args, x);
    //testing
    //printf("Value of x: %d\n", x);
    for(i=x; i<512; i++){
      args[i] = malloc(sizeof(char)*256);
    }
    //printf("status: %d\n", status);
  }

  free(line);
  free(cmd);
  for(i=0; i<512; i++)  free(args[i]);


}


int main(){

  smallsh();


  return 0;
}
