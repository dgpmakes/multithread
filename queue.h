#ifndef HEADER_FILE
#define HEADER_FILE
#include <semaphore.h>


struct element {
  int type; //Machine type
  int time; //Using time
};

typedef struct queue {

  int size;
  struct element** store_array;
  int head;
  int tail;
  sem_t semaphore;

} queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
