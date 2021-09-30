/**
 * @file bqueue.h
 * @brief Binary queue type declaration
 * @author Vikman Fernandez-Castro
 * @date September 21, 2021
 */

#ifndef BQUEUE_H
#define BQUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define BQUEUE_WAIT     1   ///< Block push if the queue is full, or pop/peek if the queue is empty
#define BQUEUE_SHRINK   2   ///< Shrink queue on pop/peek, if more than a half of the queue is unused

/**
 * @brief Binary string queue type
 *
 * Circular buffer which holds binary strings (byte-arrays). The data is not delimited or zero-terminated.
 * The maximum capacity is @p max_length - 1, so the minimum value for @p max_length is 2.
 * Operations are serialized by mutual exclusion.
 */
typedef struct {
    void * memory;              ///< Pointer to the data segment
    void * head;                ///< Pointer to the first avilable byte to read
    void * tail;                ///< Pointer to the first available byte write
    size_t length;              ///< Current queue length
    size_t max_length;          ///< Maximum length limit
    unsigned flags;             ///< Queue-wide flag set
    pthread_mutex_t mutex;      ///< Internal lock
    pthread_cond_t cond_pushed; ///< Some data has been pushed, pop/peek may be available
    pthread_cond_t cond_popped; ///< Some data has been popped, push may be available
} bqueue_t;

/**
 * @brief Allocate and inititialize a new queue
 *
 * @param max_length Maximum allowed length of the queue.
 * @param flags Queue options: @p 0 or @p BQUEUE_SHRINK.
 * @return Pointer to a newly allocated queue.
 * @retval NULL No queue allocated, parameter value error.
 */
bqueue_t * bqueue_init(size_t max_length, unsigned flags);

/**
 * @brief Free a queue
 *
 * @param queue Pointer to a queue, @p NULL is allowed.
 */
void bqueue_destroy(bqueue_t * queue);

/**
 * @brief Push data into the queue
 *
 * @param queue Pointer to a queue.
 * @param data Pointer to the source data.
 * @param length Number of bytes to insert.
 * @param flags Operation options: @p 0 or @p BQUEUE_WAIT.
 * @retval 0 On success.
 * @retval -1 No sparece available.
 */
int bqueue_push(bqueue_t * queue, const void * data, size_t length, unsigned flags);

/**
 * @brief Get and remove data from the queue
 *
 * Read at most length bytes from the queue and make their space available for
 * upcoming insertions. If @p BQUEUE_WAIT defined, this operation shall block
 * until at less one byte is readable.
 *
 * If @p BQUEUE_SHRINK was defined on initialization, the queue will be resized
 * down when the used space is less than half of the current capacity. In any
 * case, the internal buffer will be deallocated if the queue gets empty.
 *
 * This operation is equivalent to call bqueue_peek() + bqueue_drop() atomically.
 *
 * @param queue Pointer to a queue.
 * @param[out] buffer Pointer to the destination buffer.
 * @param length Maximum number of bytes that should be popped.
 * @param flags Operation options: @p 0 or @p BQUEUE_WAIT.
 * @return Number of bytes popped from the queue.
 */
size_t bqueue_pop(bqueue_t * queue, void * buffer, size_t length, unsigned flags);

/**
 * @brief Get data from the queue
 *
 * Read at most length bytes from the queue but do not remove them.
 * If @p BQUEUE_WAIT defined, this operation shall block until at less one byte
 * is readable.
 *
 * @param queue Pointer to a queue.
 * @param[out] buffer Pointer to the destination buffer.
 * @param length Maximum number of bytes that should be peeked.
 * @param flags Operation options: @p 0 or @p BQUEUE_WAIT.
 * @return Number of bytes read from the queue.
 */
size_t bqueue_peek(bqueue_t * queue, char * buffer, size_t length, unsigned flags);

/**
 * @brief Drop data from the queue
 *
 * Remove length bytes from the queue. That requires that the queue contains at
 * less length bytes.
 *
 * If @p BQUEUE_SHRINK was defined on initialization, the queue will be resized
 * down when the used space is less than half of the current capacity. In any
 * case, the internal buffer will be deallocated if the queue gets empty.
 *
 * @param queue Pointer to a queue.
 * @param length Number of bytes that shall be removed.
 * @retval 0 On success.
 * @retval -1 length is greater than the currently used space.
 */
int bqueue_drop(bqueue_t * queue, size_t length);

#endif
