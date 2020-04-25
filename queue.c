#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <semaphore.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){

    queue * q = (queue *) malloc(sizeof(queue));

    if (q == NULL) return NULL;

    /* Initialize the queue */
    q->size = size;
    q->store_array = (struct element **) malloc(sizeof(struct element*) * size);
    
    //Initialize the array
    for(int i = 0; i < size; i++){
        q->store_array[i] = NULL;
    }

    q->head = 0;
    q->tail = 0;

    //Initialize the semaphore
    sem_init(&q->semaphore, 0, 1);

    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {


    while(queue_full(q)){
        //Wait
    };

    
    if(queue_empty(q)){
        sem_wait(&q->semaphore);
        q->store_array[q->tail] = x;
        sem_post(&q->semaphore);

        return 0;
    }
    
    sem_wait(&q->semaphore);

    q->store_array[(q->tail + 1) % q->size] = x;
    q->tail = ((q->tail + 1) % q->size);

    sem_post(&q->semaphore);


    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {

    while(queue_empty(q)){
        //Wait
    };

    sem_wait(&q->semaphore);

    struct element* el = q->store_array[q->head];
    q->store_array[q->head] = NULL;
    if(q->head != q->tail) q->head = (q->head + 1) % q->size;

    sem_post(&q->semaphore);
    
    return el;

}


//To check queue state
int queue_empty(queue *q){

    sem_wait(&q->semaphore);
    
    int ret = q->head == q->tail 
    && q->store_array[q->head] == NULL
    && q->store_array[q->tail] == NULL;
    
    sem_post(&q->semaphore);

    return ret;

}

int queue_full(queue *q){

    sem_wait(&q->semaphore);
    int ret  = (q->head - q->tail + q->size) % q->size == 1;
    sem_post(&q->semaphore);

    return ret;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    
    //sem_wait(&q->semaphore);


    /* Free each element struct in the array */
    // for(int i = 0; i < sizeof(q->store_array)/sizeof(q->store_array[0]); i++){
    //     if(q->store_array[i] != NULL) free(q->store_array[i]);
    // }

    free(q->store_array);
    free(q);


    //sem_post(&q->semaphore);

    return 0;

}
