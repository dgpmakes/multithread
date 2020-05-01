
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

//Function declaration
void printQ(queue* test);
void* producer(void* producer_args);
void* consumer(void* consumer_args);

struct producer_args {
    int start;
    int end;
    struct element* lines;
    queue* q;
};

struct consumer_args {
    int line_count;
    int* total_sum;
    queue* q;
};

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) { //command, file_name, num_producers, buff_size

    

    /* First of all, we check the arguments */

    if(argc != 4){
        perror("Invalid number of arguments.\n");
        return -1;
    }

    if(atoi(argv[2])<1 || atoi(argv[3])<1){
        perror("Invalid parameters type/size.\n");
        return -1;
    }

    //printf("DEBUG: checked arguments \n");

   
    /* It is checked that the file exists and
      the number of producers do not exceed the content data. */

    FILE* openedFile = fopen(argv[1], "r");

    if(openedFile == NULL){
        perror("File does not exist.\n");
        return -1;
    } 

        //printf("DEBUG: opened file \n");


    char firstLine[256];
    int line_count;

    fgets(firstLine, 256, openedFile);

    if(atoi(firstLine) == 0){
        perror("Invalid total number of lines.\n");
        return -1;
    }
    line_count = atoi(firstLine);

    if(line_count < atoi(argv[2])) perror("There can not be more producers than entries.\n");

        //printf("DEBUG: checked entries number of file with lines count: %i\n", line_count);

    /* Load file in memory line per line */

    struct element* lines = malloc(sizeof(struct element) * line_count);
    char readLine[256];
    char * delimiter = " ";
    for(int i = 0; i < line_count; i++)
    {
        char* checkError = fgets(readLine, 256, openedFile);
        if(checkError == NULL && feof(openedFile)){ perror("Unexpected EOF.\n"); return 1;}
        
        //First get the index
        int elements = atoi(strtok(readLine, delimiter));
        if(i+1 != elements) 
            perror("Unexpected index mismatch.\n");
            
        int type = atoi(strtok(NULL, delimiter));
        lines[i].type = type;
        int time = atoi(strtok(NULL, delimiter));
        lines[i].time = time;

    }
    fclose(openedFile);
        //printf("DEBUG: loaded file in memory \n");


    //printf("\n");



    //test
    for(int i = 0; i < line_count; i++){
        //printf("%i{type: %i, time: %i}\n", i, lines[i].type, lines[i].time);
    }

    /* Distribution of lines per producer */
    
    int num_producers = atoi(argv[2]);
    int circular_queue_size = atoi(argv[3]);
    
    //The queue all the threads will use
    queue* main_queue = queue_init(circular_queue_size);
        //printQ(main_queue);
        //printf("DEBUG: created queue, starting threads... \n");

    //Create producers threads
    pthread_t threads[num_producers];
    //struct producer_args* p_args = (struct producer_args*) malloc(sizeof(struct producer_args) * num_producers);
    struct producer_args p_args [num_producers];

    for(int i = 0; i < num_producers - 1; i++){
        p_args[i].start = i*(line_count/num_producers);
        p_args[i].end =  p_args[i].start + (line_count/num_producers) - 1;
        p_args[i].lines = lines;
        p_args[i].q = main_queue;
        pthread_create(&threads[i], NULL, producer, &p_args[i]);
        //printf("DEBUG: created producer thread %i\n", i);

    }
    //Last producer threads takes the leftovers
    p_args[num_producers - 1].start = (num_producers - 1)*(line_count/num_producers);
    p_args[num_producers - 1].end =  line_count - 1;
    p_args[num_producers - 1].lines = lines;
    p_args[num_producers - 1].q = main_queue;
    pthread_create(&threads[num_producers - 1], NULL, producer, &p_args[num_producers - 1]);
    //printf("DEBUG: created producer thread %i\n", num_producers - 1);

    //Create the consumer
    int* total_sum = (int*) malloc(sizeof(int));
    *total_sum = 0;
    struct consumer_args* c_args = (struct consumer_args*) malloc(sizeof(struct consumer_args));
    c_args->total_sum = total_sum;
    c_args->q = main_queue;
    c_args->line_count = line_count;
    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, consumer, (void*) c_args);

    // Join all the threads
    for(int i = 0; i < num_producers; i++){

        pthread_join(threads[i], NULL);
    }

    pthread_join(consumer_thread, NULL);

    printf("Total: %i €.\n", *total_sum);

    queue_destroy(main_queue);
    
    return 0;
}


void* producer(void* args){
    struct producer_args* p_args = args;
    
    for(int i = 0; i < p_args->end - p_args->start + 1; i++){
        queue_put(p_args->q, p_args->lines[i + p_args->start]);
    }

    pthread_exit(NULL);

    return NULL;

}

void* consumer(void* args){

    struct consumer_args* c_args = (struct consumer_args*) args;

    for(int i = 0; i < c_args->line_count; i++){
        struct element result = queue_get(c_args->q);
        

        switch (result.type) {
        case 1:
            *(c_args->total_sum) += result.time;
            break;
        case 2:
            *(c_args->total_sum) += 3*result.time;
            break;
        case 3:
            *(c_args->total_sum) += 10*result.time;
            break;
        default:
            printf("The machine is not defined.%i%i\n", result.type, result.time);
            break;
        }

    }

    pthread_exit(NULL);

    return 0;

}






























void printQ(queue* test){

    printf("Size: %i\n", test->size);
    printf("Head: %i\n", test->head);
    printf("Tail: %i\n", test->tail);

    printf("[");

    for(int i = 0; i < test->size; i++){
        if(test->store_array[i].type != -1){
            printf("(%i) ", i);
            if(test->head == i) printf("|H|");
            if(test->tail == i) printf("|T|");

            printf("{type %i time %i}", test->store_array[i].type, test->store_array[i].time);
            
            if(test->head == i) printf("|H|");
            if(test->tail == i) printf("|T|");

            printf(", ");
        }
    }
    printf("\b\b");
    printf("]\n");
}