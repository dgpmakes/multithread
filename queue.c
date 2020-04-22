#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"


//To create a queue
queue* queue_init(int size){

    queue * q = (queue *) malloc(sizeof(queue));

    if (q == NULL) return NULL;

    /* Initialize the queue */
    q->size = size;
    q->store_array = (struct element **) malloc(sizeof(struct element*) * size);
    q->head = 0;
    q->tail = 0;

    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {

    if (queue_full(q)) return -1;
    
    if(queue_empty(q)){
        q->store_array[q->tail] = x;
        return 0;
    }

    q->store_array[(q->tail + 1) % q->size] = x;
    q->tail = ((q->tail + 1) % q->size);

    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {

    if (queue_empty(q)) return NULL;

    struct element* el = q->store_array[q->head];
    q->store_array[q->head] = NULL;
    if(q->head != q->tail) q->head = (q->head + 1) % q->size;
    
    return el;

}


//To check queue state
int queue_empty(queue *q){
    
    return q->head == q->tail 
    && q->store_array[q->head] == NULL
    && q->store_array[q->tail] == NULL;

}

int queue_full(queue *q){
    
    return (q->head - q->tail + q->size) % q->size == 1;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){

    /* Free each element struct in the array */
    for(int i = 0; i < sizeof(q->store_array)/sizeof(q->store_array[0]); i++){
        if(q->store_array[i] != NULL) free(q->store_array[i]);
    }

    free(q->store_array);
    free(q);
    return 0;

}
