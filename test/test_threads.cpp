// Vikman
// April 11, 2021

#include "mqueue.hpp"
#include <iostream>
#include <cstring>
#include <thread>

using namespace std;

static void run_reader(MQueue * queue) {
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), stdin)) {
        char * newline = strchr(buffer, '\n');

        if (newline == NULL) {
            cerr << "WARN: Line exceeding size limit. Discarding.\n";
            abort();
            continue;
        }

        *newline = '\0';
        queue->push(buffer, MQueue::Wait);
    }

    queue->push("\\0EOF\\0", MQueue::Wait);
}

static void run_writer(MQueue * queue) {
    char buffer[4096];

    queue->pop(buffer, sizeof(buffer), MQueue::Wait);

    while (strcmp(buffer, "\\0EOF\\0") != 0) {
        cout << buffer << endl;
        queue->pop(buffer, sizeof(buffer), MQueue::Wait);
    }
}

int main(int argc, char ** argv) {
    long max_length = argc > 1 ? atol(argv[1]) : 4096;

    if (max_length <= 1) {
        cerr << "WARN: Invalid size parameter. It must be greater than 1.\n";
        max_length = 4096;
    }

    MQueue queue(max_length, MQueue::Shrink);

    thread reader(run_reader, &queue);
    thread writer(run_writer, &queue);

    reader.join();
    writer.join();

    return EXIT_SUCCESS;
}
