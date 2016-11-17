/*Nick Wong
 Tuesday, November 15th
 OS-Class assign3
 smallsh.c
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  //testing
  int i;
  //for(i=0; i<512; i++)
  //  printf("arg %d: %s\n", i, args[i]);

  pid_t spawnpid = fork();
  if(spawnpid==0){  //child
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
  //memset(args, '\0', 512);
  //for(i=0; i<512; i++)  memset(args[i], '\0', 256); //same for arguments
  //for(i=0; i<5; i++)  args[i] = NULL;

  //testing
  //int j;
  /*for(i=0; i<512; i++){
    printf("arg %d: %s\n", i, args[i]);
    //if(args[i][0]!='\0')  printf("arg %d: %s\n", i, args[i]);
    //else  printf("arg %d: %d\n", i, 0)
    //for(j=0; j<256; j++){
    //}
  }*/

  //testing
  //printf("Line: %s\n", *line);

  //store line as arguments
  //testing
  //printf("size of arg[0]: %d\n", sizeof(args[0]));
  //snprintf(args[0], sizeof(args[0]), "%s", strtok(*line,"\n ,-"));    //initial strtok
  snprintf(args[0], 50, "%s", strtok(*line,"\n ,-"));    //initial strtok
  while(strcmp(args[x], "(null)")!=0){  //will copy in string "(null)" (dont know why)
    x++;
    //snprintf(args[x], sizeof(args[x]), "%s", strtok(NULL,"\n ,-"));   //every additional strtok (null, we already opened)
    snprintf(args[x], 50, "%s", strtok(NULL,"\n ,-"));
    //printf("arg[%d]: %s\n", x, args[x]);
  }
  //store first arg as cmd
  strcpy(*cmd, args[0]);
  //testing
  //printf("cmd: %s\n", *cmd);
  //printf("X: %d\n", x);
  /*for(i=0; args[i][0]!='\0'; i++){
    printf("Arg %d: %s\n", i, args[i]);
  }

  for(i=0; args[i][0]!='\0'; i++){
    printf("Arg %d: %s\n", i, args[i]);
  }*/
  //memset(args[i-1], '\0', 256);   //remove that (null) arg
  //printf("memset\n");
  //this will set the args after last one to NULL so that execvp knows when to stop
  int temp=x;
  //for(i=x; i<512; i++){
  //  free(args[i]);
  //  args[i] = NULL;
    //printf("arg %d: %s\n", i, args[i]);
  //}
  //for(i=0; i<512; i++){
   //   if(args[i]!=NULL) printf("arg %d: %s\n", i, args[i]);
    //}

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
