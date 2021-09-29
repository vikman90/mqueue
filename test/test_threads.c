// Vikman
// April 11, 2021

#include "bqueue.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void * run_reader(void * arg) {
    bqueue_t * queue = (bqueue_t *)arg;
    char buffer[4096];
    int r __attribute__((unused));

    while (fgets(buffer, sizeof(buffer), stdin)) {
        char * newline = strchr(buffer, '\n');

        if (newline == NULL) {
            fprintf(stderr, "WARN: Line exceeding size limit. Discarding.\n");
            abort();
            continue;
        }

        r = bqueue_push(queue, buffer, strlen(buffer), BQUEUE_WAIT);
        assert(r == 0);
    }

    r = bqueue_push(queue, "\\0EOF\\0", 7, BQUEUE_WAIT);
    assert(r == 0);

    (void)r;
    return NULL;
}

static void * run_writer(void * arg) {
    bqueue_t * queue = (bqueue_t *)arg;
    char buffer[64];
    size_t len;
    int r __attribute__((unused));

    while (len = bqueue_peek(queue, buffer, sizeof(buffer), BQUEUE_WAIT), len > 0) {
        r = bqueue_drop(queue, len);
        assert(r == 0);

        if (len >= 7 && strncmp(buffer + len - 7, "\\0EOF\\0", 7) == 0) {
            printf("%.*s", (int)len - 7, buffer);
            break;
        }

        printf("%.*s", (int)len, buffer);
    }

    return NULL;
}

int main(int argc, char ** argv) {
    int r __attribute__((unused));
    long max_length = argc > 1 ? atol(argv[1]) : 4096;

    if (max_length <= 1) {
        fprintf(stderr, "WARN: Invalid size parameter. It must be greater than 1.\n");
        max_length = 4096;
    }

    bqueue_t * queue = bqueue_init(max_length, BQUEUE_SHRINK);
    assert(queue != NULL);

    pthread_t reader;
    pthread_t writer;

    r = pthread_create(&reader, NULL, run_reader, (void*)queue);
    assert(r == 0);

    r = pthread_create(&writer, NULL, run_writer, (void*)queue);
    assert(r == 0);

    r = pthread_join(reader, NULL);
    assert(r == 0);

    r = pthread_join(writer, NULL);
    assert(r == 0);

    bqueue_destroy(queue);
    return EXIT_SUCCESS;
}
