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

typedef int bool;
#define true 1
#define false 0

int status=-5;

void execExit(){
  //printf("In Exit\n");
  status=0;
}

void execCd(char path[256]){
  //printf("IN CD\n");
  chdir(path);
}

void execStatus(){
  printf("Status: %d\n", status);
  //printf("In STATUS\n");
}

//fork
void execLs(){
  char *args[2];

    args[0] = "/bin/ls";        // first arg is the full path to the executable
    args[1] = NULL;             // list of args must be NULL terminated
    //if ( fork() == 0 )
        execvp( args[0], args ); // child: call execv with the path and the args
    //else
        //wait( &status );        // parent: wait for the child (not really necessary)
}

void execCat(char tokens[256][256]){
  //if(fork()==0) execvp(tokens[0], tokens);
  //else{
    //printf("parent will wait\n";);
    //waitpid();
  //}
}

//will select the command to exec
void execCommand(char tokens[256][256], int numTokens){
  int i;
  /*testing tokens (keep for a while)
  for(i=0; i<10; i++){
    printf("token %d: %s\n", i, tokens[i]);
  }*/
  if(strcmp(tokens[0], "exit")==0)  execExit();
  else if(strcmp(tokens[0], "cd")==0)  execCd(tokens[i]); //assuming the path will be second token
  else if(strcmp(tokens[0], "status")==0)  execStatus();
  else if(strcmp(tokens[0], "ls")==0) execLs();
  else if(strcmp(tokens[0], "cat")==0) execCat(tokens);
}
//separates the input string into *tokens by whitespace
int parseInput(char *input, char tokens[256][256], char *cmd, char **args){
  int i, x=0;
  for(i=0; i<256; i++)  //safetly add null terminators
    memset(tokens[i], '\0', 256);
  //splits string into tokens, separated by whitespace
  snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(input,"\n ,-"));

  //store the first token as commands
  cmd=malloc(sizeof(char)*sizeof(tokens[0]));
  memset(cmd, '\0', sizeof(tokens[0]));
  snprintf(cmd, sizeof(tokens[0]), "%s", tokens[0]);
  //testing
  //printf("command: %s\n", cmd);

  args=malloc(sizeof(char**)*512);  //max of 512 args
  *args=malloc(sizeof(char*)*256); //have max arg length 256
  printf("size of args: %lu\n", sizeof(args));
  for(i=0; i<sizeof(args); i++){
    printf("length of args: %lu\n", sizeof(args[i]));
  }

  //for(i=0; i<sizeof(args); i++)  memset(args[i], '\0', sizeof(args[i]));
  i=0;
  while (strcmp(tokens[x], "(null)") != 0)  //when copying strings, it will copy (null)
  {
    x++;
    snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(NULL,"\n ,-"));
    snprintf(args[i], sizeof(tokens[x]), "%s", tokens[x]);
    i++;
  }

  //testing:
  for(i=0; i<sizeof(args); i++){
    printf("args: %s\n", args[i]);
  }
  return x;
}

//main shell loop, will return to main upon any error or exit()
void smallsh(){
  int numTokens;
  size_t bufsize = 0;
  char *input = NULL;
  char tokens[256][256];
  char *cmd;
  char **args;

  while(status){
    //testing
    //printf("status: %d\n", status);
    printf(": ");
    getline(&input, &bufsize, stdin);   //automatically allocs mem for input
    numTokens=parseInput(input, tokens, cmd, args);
    execCommand(tokens, numTokens);
  }



}

int main(){

  smallsh();

  //should clean up the shell here, assuming we have exited, or error has ocurred

    return 0;
}
