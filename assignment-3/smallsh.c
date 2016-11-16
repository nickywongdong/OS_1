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


void parseInput(char input[2048]){
  

}

void smallsh(){
  int status=-5;
  char input[2048];
  while(status){
    memset(input, '\0', 2048);
    getline(&input, stdin);
    parseInput(input);
    execCommand();
  }



}

int main(){

  smallsh();

    return 0;
}
