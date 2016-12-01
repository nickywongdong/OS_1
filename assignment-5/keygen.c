#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){
   if(argc!=2)	exit(1);		//not enough arguments
   
   int num=atoi(argv[1]);
   //testing
   printf("rand int: %d\n", num);



   return 0;
}
