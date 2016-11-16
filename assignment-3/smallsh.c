/*Nick Wong
 Tuesday, November 15th
 OS-Class assign3
 smallsh.c
 */

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

int status=-5;

int execExit(){
  status=0;
  return 0;
}

//will select the command to exec
void execCommand(char tokens[256][256], int numTokens){
  int i;
    if(strcmp(tokens[0], "exit")==0)  execExit();
    //else if(strcmp(tokens[0], "cd")==0)  execCd();
    //else if(strcmp(tokens[0], "status")==0)  execStatus();
}
//separates the input string into *tokens by whitespace
int parseInput(char *input, char tokens[256][256]){
  int i, x=0;
  for(i=0; i<256; i++)  //safetly add null terminators
    memset(tokens[i], '\0', 256);

  //splits string into tokens, separated by whitespace
  snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(input," ,.-"));
  while (strcmp(tokens[x], "(null)") != 0)  //when copying strings, it will copy (null)
  {
    x++;
    snprintf(tokens[x], sizeof(tokens[x]), "%s", strtok(NULL," ,.-"));
  }
  return x;
}

//main shell loop, will return to main upon any error or exit()
void smallsh(){
  int status=-5, numTokens;
  size_t bufsize = 0;
  char *input = NULL;
  char tokens[256][256];

  while(status){
    printf(": ");
    getline(&input, &bufsize, stdin);   //automatically allocs mem for input
    numTokens=parseInput(input, tokens);
    execCommand(tokens, numTokens);
  }



}

int main(){

  smallsh();

  //should clean up the shell here, assuming we have exited, or error has ocurred

    return 0;
}
