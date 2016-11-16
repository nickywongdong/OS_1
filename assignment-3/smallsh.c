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


void parseInput(char *input){
  printf("input: %s\n", input);
}

void smallsh(){
  int status=-5;
  size_t bufsize = 0;
  char *input = NULL;

  while(status){
    //memset(input, '\0', 2048);
    printf(": ");
    getline(&input, &bufsize, stdin);
    parseInput(input);
    //execCommand();
  }



}

int main(){

  smallsh();

    return 0;
}
