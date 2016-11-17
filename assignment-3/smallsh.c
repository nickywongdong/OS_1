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

void execCd(char *args[512]){
  //printf("IN CD\n");
  chdir(args[0]);
}

void execStatus(){
  printf("Status: %d\n", status);
  //printf("In STATUS\n");
}

//fork
void execLs(char *tokens[256], char *arguments[512]){
  pid_t spawnpid = -5;
  char *args[2];

    args[0] = "ls";        // first arg is the full path to the executable
    args[1] = NULL;             // list of args must be NULL terminated
    spawnpid=fork();
    if ( spawnpid == 0 ){
        execvp( args[0], args ); // child: call execv with the path and the args
      }

    else  {
      return;
      //wait( &status );        // parent: wait for the child (not really necessary)
    }
}

void execCat(char *tokens[256], char *arguments[512]){
  char *args[2];

  args[0] = "cat";        // first arg is the full path to the executable
  args[1] = NULL;             // list of args must be NULL terminated

  if(fork()==0){
      execvp(args[0], tokens);
  }
  else  wait(&status);
}

//will select the command to exec
void execCommand(char *tokens[256], char *cmd, char *args[512]){
  int i;
  /*testing tokens (keep for a while)
  for(i=0; i<10; i++){
    printf("token %d: %s\n", i, tokens[i]);
  }*/
  //testing
  printf("The command: %s\n", cmd);
  if(strcmp(cmd, "exit")==0)  execExit();
  else if(strcmp(cmd, "cd")==0)  execCd(args); //assuming the path will be second token
  else if(strcmp(cmd, "status")==0)  execStatus();
  else if(strcmp(cmd, "ls")==0) execLs(tokens, args);
  else if(strcmp(cmd, "cat")==0) execCat(tokens, args);
  //else  printf("command not found\n");
}


//separates the input string into *tokens by whitespace
char *parseInput(char *input, char *tokens[256], char *args[512]){
  char *cmd;
  int i, x=0;
  for(i=0; i<256; i++){
    tokens[i]=malloc(sizeof(char)*256);
  }
  for(i=0; i<512; i++){  //safetly add null terminators
    args[i]=malloc(sizeof(char)*256);
    memset(args[i], '\0', 256);
  }

  for(i=0; i<256; i++)  //safetly add null terminators
    memset(tokens[i], '\0', 256);
  //splits string into tokens, separated by whitespace
  snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(input,"\n ,-"));

  //store the first token as commands
  cmd=malloc(sizeof(char)*sizeof(tokens[0]));
  memset(cmd, '\0', sizeof(tokens[0]));
  snprintf(cmd, sizeof(tokens[0]), "%s", tokens[0]);



  for(i=0; i<512; i++)  memset(args[i], '\0', 256);
  i=0;
  while (strcmp(tokens[x], "(null)") != 0)  //when copying strings, it will copy (null)
  {
    x++;
    snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(NULL,"\n ,-"));
    snprintf(args[i], sizeof(tokens[x]), "%s", tokens[x]);
    i++;
  }

  /*testing:
  for(i=0; strcmp(args[i], "(null)")!=0; i++){
    printf("args: %s\n", args[i]);
  }*/
  return cmd;
}

//main shell loop, will return to main upon any error or exit()
void smallsh(){
  //int numTokens;
  size_t bufsize = 0;
  char *input = NULL;
  char *tokens[256];
  char *cmd;
  char *args[512];  //max 512 args, 256 in length

  while(status){
    //testing
    //printf("status: %d\n", status);
    printf(": ");
    fflush(stdout);
    fflush(stdin);
    getline(&input, bufsize, stdin);   //automatically allocs mem for input
    cmd=parseInput(input, tokens, args);  //or pass cmd by reference
    execCommand(tokens, cmd, args);
  }



}

int main(){

  smallsh();

  //should clean up the shell here, assuming we have exited, or error has ocurred

    return 0;
}
