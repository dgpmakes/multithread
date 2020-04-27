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
    q->store_array = (struct element *) malloc(sizeof(struct element) * size);
    

    //Initialize the array
    for(int i = 0; i < size; i++){
        q->store_array[i].type = -1;
        q->store_array[i].time = -1;
    }

    q->head = 0;
    q->tail = 0;

    //Initialize the semaphore
    pthread_mutex_init(&q->mutex, NULL);

    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element x) {

    int repeat = 1;
    while(repeat){
        pthread_mutex_lock(&q->mutex);
        repeat = queue_full(q);
        if(repeat == 0){
            break;
        } else {
            pthread_mutex_unlock(&q->mutex);
        }
    };
    
    if(queue_empty(q)){
        q->store_array[q->tail] = x;
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }
    
    q->store_array[(q->tail + 1) % q->size] = x;
    q->tail = ((q->tail + 1) % q->size);

    pthread_mutex_unlock(&q->mutex);

    return 0;
}


// To Dequeue an element.
struct element queue_get(queue *q) {

    //pthread_mutex_lock(&q->mutex);
    int repeat = 1;
    while(repeat){
        pthread_mutex_lock(&q->mutex);
        repeat = queue_empty(q);
        if(repeat == 0){
            break;
        } else {
            pthread_mutex_unlock(&q->mutex);
        }
    };

    struct element el = q->store_array[q->head];
    q->store_array[q->head].type = -1;
    q->store_array[q->head].time = -1;
    if(q->head != q->tail) q->head = (q->head + 1) % q->size;

    pthread_mutex_unlock(&q->mutex);
    
    return el;

}


//To check queue state
int queue_empty(queue *q){

    //pthread_mutex_lock(&q->mutex);
    
    int ret = q->head == q->tail 
    && q->store_array[q->head].type == -1
    && q->store_array[q->tail].type == -1;
    
    //pthread_mutex_unlock(&q->mutex);

    return ret;

}

int queue_full(queue *q){

    //pthread_mutex_lock(&q->mutex);

    int ret = (q->head - q->tail + q->size) % q->size == 1;

    //pthread_mutex_unlock(&q->mutex);

    return ret;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    


    /* Free each element struct in the array */
    // for(int i = 0; i < sizeof(q->store_array)/sizeof(q->store_array[0]); i++){
    //     if(q->store_array[i] != NULL) free(q->store_array[i]);
    // }
    pthread_mutex_destroy(&q->mutex);
    free(q->store_array);
    free(q);



    return 0;

}
