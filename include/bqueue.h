// September 21, 2021

#ifndef BQUEUE_H
#define BQUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define BQUEUE_WAIT     1
#define BQUEUE_SHRINK   2

typedef struct {
    void * memory;
    void * head;
    void * tail;
    size_t length;
    size_t max_length;
    unsigned flags;
    pthread_mutex_t mutex;
    pthread_cond_t cond_pushed;
    pthread_cond_t cond_popped;
} bqueue_t;

bqueue_t * bqueue_init(size_t max_length, unsigned flags);
void bqueue_destroy(bqueue_t * queue);
int bqueue_push(bqueue_t * queue, const void * data, size_t length, unsigned flags);
size_t bqueue_pop(bqueue_t * queue, void * buffer, size_t length, unsigned flags);
size_t bqueue_peek(bqueue_t * queue, char * buffer, size_t length, unsigned flags);
int bqueue_drop(bqueue_t * queue, size_t length);

#endif
