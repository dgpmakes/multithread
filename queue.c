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

    /*
    Check is the queue is full
    A while is used due to a limitation in the concurreny control:
    if the waiting thread is signaled that now it can put an element in the queue,
    the waiting thread does not execute inmediately after the signaling thread,
    it might be possible that another thread inserts an element before the signaled one.
    If this happens, then the thread will insert one element after the wait and the
    queue might be full!
    This is why it doubles-check if the queue is not full right after the thread is resumed.
    */
    while(q->current_size == q->size){

        pthread_cond_wait(&q->full, &q->mutex);

    }

    //Check if the queue is empty.
    if(q->current_size == 0){

        //The algorithm to insert one element when
        // the queue is empty is different
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

    /*
    Check is the queue is empty
    A while is used due to a limitation in the concurreny control:
    if the waiting thread is signaled that now it can get an element from the queue,
    the waiting thread does not execute inmediately after the signaling thread,
    it might be possible that another thread gets an element before the signaled one.
    If this happens, then the thread will get one element after the wait and the
    queue might be already empty!
    This is why it doubles-check if the queue is not empty right after the thread is resumed.
    */
    while(q->current_size == 0){

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
    pthread_mutex_lock(&q->mutex);
    int result = q->current_size == 0;
    pthread_mutex_unlock(&q->mutex);

    return result;


}

int queue_full(queue *q){
    pthread_mutex_lock(&q->mutex);
    int result = q->current_size == q->size;
    pthread_mutex_unlock(&q->mutex);

    return result;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    
    free(q->store_array);
    free(q);

    return 0;

}