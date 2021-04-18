// Vikman
// April 11, 2021

#include "mqueue.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void * run_reader(void * arg) {
    mqueue_t * queue = (mqueue_t *)arg;
    char buffer[4096];
    int r __attribute__((unused));

    while (fgets(buffer, sizeof(buffer), stdin)) {
        char * newline = strchr(buffer, '\n');

        if (newline == NULL) {
            fprintf(stderr, "WARN: Line exceeding size limit. Discarding.\n");
            abort();
            continue;
        }

        *newline = '\0';
        r = mqueue_push(queue, buffer, MQUEUE_WAIT);
        assert(r == 0);
    }

    r = mqueue_push(queue, "\\0EOF\\0", MQUEUE_WAIT);
    assert(r == 0);

    (void)r;
    return NULL;
}

static void * run_writer(void * arg) {
    mqueue_t * queue = (mqueue_t *)arg;
    char buffer[4096];

    while (mqueue_pop(queue, buffer, sizeof(buffer), MQUEUE_WAIT) == 0) {
        if (strcmp(buffer, "\\0EOF\\0") == 0) {
            break;
        }

        printf("%s\n", buffer);
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

    mqueue_t * queue = mqueue_init(max_length, MQUEUE_THREADS | MQUEUE_SHRINK);
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

    mqueue_destroy(queue);
    return EXIT_SUCCESS;
}
