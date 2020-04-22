
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

    queue* test = queue_init(4);

    struct element elementTest1;
    elementTest1.time = 1221;
    elementTest1.type = 1212;

    struct element elementTest2;
    elementTest2.time = 19;
    elementTest2.type = 19;

    struct element elementTest3;
    elementTest3.time = 13;
    elementTest3.type = 99;

    struct element elementTest4;
    elementTest4.time = 1;
    elementTest4.type = 69;

    struct element elementTest5;
    elementTest5.time = 30;
    elementTest5.type = 4;

    queue_put(test, &elementTest1);
    queue_put(test, &elementTest2);
    queue_put(test, &elementTest3);
    queue_put(test, &elementTest4);
    printQ(test);
    //queue_put(test, &elementTest5);
    queue_get(test);
    printQ(test);
    queue_get(test);
    queue_get(test);
    printQ(test);

    //Get last element
    queue_get(test);
    printQ(test);

    //Set elements from zero phase
    queue_put(test, &elementTest3);
    printQ(test);
    queue_put(test, &elementTest1);
    printQ(test);

    queue_get(test);
    printQ(test);

    //Empty again
    queue_get(test);
    printQ(test);

    //More empty when already empty
    queue_get(test);
    queue_get(test);
    queue_get(test);
    queue_get(test);
    printQ(test);

    //Inser again elements
    queue_put(test, &elementTest3);
    queue_put(test, &elementTest2);
    printQ(test);
    queue_put(test, &elementTest1);
    printQ(test);

    //Now test overinsertion
    queue_put(test, &elementTest4);
    printQ(test);
    int result = queue_put(test, &elementTest2);
    printf("%i\n", result);
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

    printf("Size: %i\n", test->size);
    printf("Head: %i\n", test->head);
    printf("Tail: %i\n", test->tail);

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