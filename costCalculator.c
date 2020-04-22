
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define NUM_CONSUMERS 1



/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {

    /*
        First of all, we check the arguments
    */

   if(argc != 4) perror("Invalid number of arguments.");
   
   /*
        It is checked that the file exists and
        the number of producers do not exceed the content data.
    */

   int descr = fopen(argv[1],'r');
   if(descr==NULL) perror("Invalid number of arguments.");
    char* amount;
    fgets();
    
    int total = 0;
    printf("Total: %i €.\n", total);

    return 0;
}
