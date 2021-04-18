// Vikman
// April 9, 2021

#ifndef MQUEUE_H
#define MQUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define MQUEUE_THREADS  1
#define MQUEUE_WAIT     2
#define MQUEUE_SHRINK   4

typedef struct {
    char * memory;
    char * head;
    char * tail;
    size_t length;
    size_t max_length;
    unsigned flags;
    pthread_mutex_t mutex;
    pthread_cond_t cond_pushed;
    pthread_cond_t cond_popped;
} mqueue_t;

mqueue_t * mqueue_init(size_t max_length, unsigned flags);
void mqueue_destroy(mqueue_t * queue);
int mqueue_push(mqueue_t * queue, const char * data, unsigned flags);
int mqueue_pop(mqueue_t * queue, char * buffer, size_t length, unsigned flags);

size_t _mqueue_used(mqueue_t * queue);
bool _mqueue_empty(mqueue_t * queue);
void _mqueue_expand(mqueue_t * queue, size_t new_length);
void _mqueue_shrink(mqueue_t * queue, size_t new_length);
void _mqueue_insert(mqueue_t * queue, const char * data, size_t data_len);
void _mqueue_extract(mqueue_t * queue, char * buffer, size_t buffer_len);
void _mqueue_trim(mqueue_t * queue);

#endif
