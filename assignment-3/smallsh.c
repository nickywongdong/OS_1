/*Nick Wong
 Tuesday, November 15th
 OS-Class assign3
 smallsh.c
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
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

char inFile[50];
char outFile[50];

int status=-5;    //overall status    (can change from global later if we run into problems)
int displayedStatus=0;  //status that we print out (different from the status due to while loop in main)


//simply exits the shell, will set status to 0 (breaking out of loop)
void execExit(){
  status=0;
}

//runs cd build in command
void execCd(char *cmd, char*args[]){
  char *home = getenv("HOME");
  //printf("in cd\n");
  //printf("args at 1: %s\n", args[1]);
  if(!strcmp(args[1], "(null)")){
    //printf("changing directory to home\n");
    chdir(home);
  }
  else if(!chdir(args[1])){
    //printf("error in cd\n");
  }
  //testing
  //printf("cmd: %s\n", cmd);
  //chdir(cmd);
  //status=0;
}

//will display status of prev commands
void execStatus(){
  printf("Status: %d\n", displayedStatus);
  displayedStatus=0;
}

//will execute all other commands based off PATH variable: /usr/built-in
void execOther(char *line, char *cmd, char *args[], int x){
  int i;

  pid_t spawnpid = fork();
  if(spawnpid==0){  //child
    int fd;
    //if our flag was set to output
    if(isOutput){
      //printf("redirecting to read\n");
      fd=open(outFile, O_RDONLY, 0777);   //open for reading
      dup2(fd, 1);    //redirect from stdout (1) to filedecriptor
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
    //status=0;
    //testing
    //printf("CMD: %s\n", cmd);
    for(i=x; i<512; i++){
      //free(args[i]);
      args[i] = NULL;
      //printf("arg %d: %s\n", i, args[i]);
    }
    execvp(cmd, args);    //simply run the cmd with the arguments ***consider redirection later
  }
  else if(spawnpid<0){    //ran into an error
    printf("ERROR\n");
    status=-1;
  }
  else{               //it's the parent
    waitpid(spawnpid, &displayedStatus, 0);     //parent wait for child to terminate
  }
}
//will execute the commands based off of cmd string (checks for built-in commands first)
void execCommands(char *line, char*cmd, char*args[], int x){
  //first we check for built-in commands
  if(strcmp(cmd, "exit")==0)  execExit();
  else if(strcmp(cmd, "cd")==0) execCd(cmd, args);
  else if(strcmp(cmd, "status")==0) execStatus();
  else  execOther(line, cmd, args, x);
}
//splits the input string by spaces, and stops at the newline
int parseInput(char **line, char **cmd, char *args[]){
  int i, x=0;
  memset(*cmd, '\0', 256);    //make sure cmd is properly init, or we reset cmd from last time

  snprintf(args[0], 50, "%s", strtok(*line,"\n ,-"));    //initial strtok
  while(strcmp(args[x], "(null)")!=0){  //will copy in string "(null)" (dont know why)
    x++;
    snprintf(args[x], 50, "%s", strtok(NULL,"\n ,-"));
    if(args[x][0]=='<' || args[x][0]=='>' || args[x][0]=='&') {
      snprintf(args[x+1], 50, "%s", strtok(NULL,"\n ,-"));      //copy the filename into args
      break;      //no longer arguments
    }
  }
  //store first arg as cmd
  strcpy(*cmd, args[0]);
  int temp=x;
  if(args[x][0]=='<'){
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
  }

  //we then return that initial x value so we can reallocate memory for what we free'ed
  return temp;
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
  char *line=malloc(sizeof(char)*2048), *cmd=malloc(sizeof(char)*256);
  char *args[512];
  for(i=0; i<512; i++)  args[i]=malloc(sizeof(char)*256);

  while(status){
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
