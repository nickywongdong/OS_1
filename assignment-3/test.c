#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

status=-5;


//void parsecmd(char *cmd, char *argv[512]){

//}


int runcmd(char *cmd, char *argv[512])
{
  //char* argv[512];
  pid_t child_pid;
  int child_status;

  //parsecmd(cmd,argv);
  child_pid = fork();
  if(child_pid == 0) {
    /* This is done by the child process. */

    execvp(argv[0], argv);

    /* If execvp returns, it must have failed. */

    printf("Unknown command\n");
    exit(0);
  }
  else {
     /* This is run by the parent.  Wait for the child
        to terminate. */

     do {
       pid_t tpid = wait(&child_status);
       if(tpid != child_pid) process_terminated(tpid);
     } while(tpid != child_pid);

     return child_status;
  }
}

//separates the input string into *tokens by whitespace
char *parseInput(char *input, char tokens[256][256], char *args[512]){
  char *cmd;
  int i, x=0;

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

void smallsh(){
  //int numTokens;
  size_t bufsize = 0;
  char *input = NULL;
  char tokens[256][256];
  char *cmd;
  char *args[512];  //max 512 args, 256 in length

  while(status){
    //testing
    //printf("status: %d\n", status);
    printf(": ");
    getline(&input, &bufsize, stdin);   //automatically allocs mem for input
    cmd=parseInput(input, tokens, args);  //or pass cmd by reference
    runcmd(cmd, args);
    //execCommand(tokens, cmd, args);
  }



}

int main(){

  smallsh();

  //should clean up the shell here, assuming we have exited, or error has ocurred

    return 0;
}
