
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

void printQ(queue* test);


/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) { //command, file_name, num_producers, buff_size

    queue* test = queue_init(10);

    struct element elementTest;
    elementTest.time = 12;
    elementTest.type = 1;

    queue_put(test, &elementTest);
    queue_put(test, &elementTest);
    queue_put(test, &elementTest);

    printQ(test);







    /* First of all, we check the arguments */

    if(argc != 4) perror("Invalid number of arguments.");
   
    /* It is checked that the file exists and
      the number of producers do not exceed the content data. */

    FILE* openedFile = fopen(argv[1], O_RDONLY);

    if(openedFile == NULL) perror("File does not exist.\n");
    char* readAmmount;
    int* ammount;
    fgets(readAmmount, 256, openedFile);
    ammount = atoi(readAmmount);
    if(ammount < argv[2]) perror("There can not be more producers than entries.\n");

    /* Calculate line blocks for producers */

    /* Load file in memory line per line */

    struct element * lines;
    char * readLine;
    char * delimiter = " ";
    for(int i = 0; i < ammount; i++)
    {
        char* checkError = fgets(readLine,256,openedFile);
        if(checkError == NULL && feof(openedFile)) perror("Unexpected EOF.\n");
        char * elements = strtok(readLine, delimiter);
        if(i != elements[0]) perror ("Unexpected index mismatch.\n");
        lines[i].type = elements[1];
        lines[i].time = elements[2];

    }

    //test
    for(int i = 0; i < 100; i++){
        printf("{type: %i, time: %i}\n", lines[i].type, lines[i].time);
    }

    /* Distribution of lines per producer */

    int threading;

    int total = 0;
    printf("Total: %i €.\n", total);

    return 0;
}


void printQ(queue* test){
    printf("[");

    for(int i = 0; i < test->size; i++){
        if(test->store_array[i] != NULL){
            printf("(%i) ", i);
            if(test->head == i) printf("|H|");
            if(test->tail == i) printf("|T|");

            printf("{type %i time %i}", test->store_array[i]->type, test->store_array[i]->time);
            
            if(test->head == i) printf("|H|");
            if(test->tail == i) printf("|T|");

            printf(", ");
        }
    }
    printf("\b\b");
    printf("]\n");
}