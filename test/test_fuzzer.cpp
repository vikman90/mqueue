// Vikman
// April 11, 2021

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <exception>
#include "mqueue.hpp"

#define QUEUE_LENGTH 4096
#define POP_TIMES    64

using namespace std;

static MQueue queue(QUEUE_LENGTH, MQueue::Shrink);

void push(const uint8_t *data, size_t size) {
    char * string = NULL;
    size_t string_size = 0;

    do {
        size_t len = strnlen((const char *)data, size);

        if (len >= string_size) {
            string_size = len + 1;
            free(string);
            string = (char *)malloc(string_size);

            if (string == NULL) {
                cerr << "FATAL: Cannot allocate memory.\n";
                abort();
            }
        }

        memcpy(string, data, len);
        string[len] = '\0';

        try {
            queue.push(string);
        } catch (MQueue::NoSpace) { }

        data += len + 1;
        size -= (size > len) ? len + 1 : len;
    } while (size > 0);

    free(string);
}

void pop(unsigned times) {
    char buffer[QUEUE_LENGTH];

    for (unsigned i = 0; i < times; i++) {
        try {
            queue.pop(buffer, QUEUE_LENGTH);
            cout << buffer << endl;
        } catch (MQueue::NoData) {
            break;
        }
    }
}

extern "C" {

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    push(data, size);
    pop(POP_TIMES);

    return 0;
}

}
