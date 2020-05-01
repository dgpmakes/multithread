#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

//To create a queue
queue* queue_init(int size){

    queue * q = (queue *) malloc(sizeof(queue));

    if (q == NULL) return NULL;

    /* Initialize the queue */
    q->size = size;
    q->current_size = 0;
    q->store_array = (struct element *) malloc(sizeof(struct element) * size);
    

    //Initialize the array
    for(int i = 0; i < size; i++){
        q->store_array[i].type = -1;
        q->store_array[i].time = -1;
    }

    q->head = 0;
    q->tail = 0;

    pthread_cond_init(&q->full, NULL);
    pthread_cond_init(&q->empty, NULL);

    pthread_mutex_init(&q->mutex, NULL);

    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element x) {

    pthread_mutex_lock(&q->mutex);

    while(queue_full(q)){

        pthread_cond_wait(&q->full, &q->mutex);

    }

    if(queue_empty(q)){
        q->store_array[q->tail] = x;
    } else {
        q->store_array[(q->tail + 1) % q->size] = x;
        q->tail = ((q->tail + 1) % q->size);
    }
    q->current_size++;

    pthread_cond_signal(&q->empty);

    pthread_mutex_unlock(&q->mutex);

    return 0;
}


// To Dequeue an element.
struct element queue_get(queue *q) {

    pthread_mutex_lock(&q->mutex);

    while(queue_empty(q)){

        pthread_cond_wait(&q->empty, &q->mutex);

    } 

    struct element el = q->store_array[q->head];
    q->store_array[q->head].type = -1;
    q->store_array[q->head].time = -1;
    if(q->head != q->tail) q->head = (q->head + 1) % q->size;
    q->current_size--;

    pthread_cond_signal(&q->full);

    pthread_mutex_unlock(&q->mutex);
    
    return el;

}


//To check queue state
int queue_empty(queue *q){

    return q->current_size == 0;
}

int queue_full(queue *q){

    return q->current_size == q->size;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    
    free(q->store_array);
    free(q);

    return 0;

}