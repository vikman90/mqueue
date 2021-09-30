// Vikman
// September 30, 2021

#include "bqueue.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static const char * SOURCE = "AB";

int main() {
    int r __attribute__((unused));

    // 1-byte queue is forbidden

    bqueue_t * queue = bqueue_init(1, 0);
    assert(queue == NULL);

    // 2-byte queue is allowed

    queue = bqueue_init(2, 0);
    assert(queue != NULL);

    // Inserting 2 bytes is forbidden

    r = bqueue_push(queue, SOURCE, strlen(SOURCE), 0);
    assert(r == -1);

    // Inserting 1 byte is allowed

    r = bqueue_push(queue, SOURCE, 1, 0);
    assert(r == 0);

    // Pop and test

    char buffer[3] = "";

    r = bqueue_peek(queue, buffer, sizeof(buffer), 0);
    assert(r == 1);
    assert(strcmp(buffer, "A") == 0);

    r = bqueue_drop(queue, 1);
    assert(r == 0);

    bqueue_destroy(queue);

    // 3-byte queue test

    queue = bqueue_init(3, 0);
    assert(queue != NULL);

    // Inserting 2 bytes is allowed

    r = bqueue_push(queue, SOURCE, strlen(SOURCE), 0);
    assert(r == 0);

    // Pop and test

    r = bqueue_peek(queue, buffer, sizeof(buffer), 0);
    assert(r == 2);
    assert(strcmp(buffer, SOURCE) == 0);

    r = bqueue_drop(queue, 2),
    assert(r == 0);

    bqueue_destroy(queue);
    return EXIT_SUCCESS;
}
