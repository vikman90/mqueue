// Vikman
// April 11, 2021

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "bqueue.h"

#define QUEUE_LENGTH 4096
#define POP_TIMES    64

static bqueue_t * queue;

void init(size_t max_length) {
    queue = bqueue_init(max_length, BQUEUE_SHRINK);

    if (queue == NULL) {
        fprintf(stderr, "FATAL: Cannot initialize queue.\n");
        abort();
    }
}

void push(const uint8_t *data, size_t size) {
    char * string = NULL;
    size_t string_size = 0;

    do {
        size_t len = strnlen((const char *)data, size);

        if (len >= string_size) {
            string_size = len;
            free(string);
            string = (char *)malloc(string_size);

            if (string == NULL) {
                fprintf(stderr, "FATAL: Cannot allocate memory.\n");
                abort();
            }
        }

        memcpy(string, data, len);
        bqueue_push(queue, string, len, 0);

        data += len + 1;
        size -= (size > len) ? len + 1 : len;
    } while (size > 0);

    free(string);
}

void pop(unsigned times) {
    char buffer[QUEUE_LENGTH];

    for (unsigned i = 0; i < times; i++) {
        size_t len = bqueue_pop(queue, buffer, QUEUE_LENGTH, 0);
        if (len > 0) {
            printf("%.*s", (int)len, buffer);
        } else {
            break;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (queue == NULL) {
        init(QUEUE_LENGTH);
    }

    push(data, size);
    pop(POP_TIMES);

    return 0;
}
