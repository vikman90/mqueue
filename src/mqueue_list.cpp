// Vikman
// April 25, 2021

#include "mqueue.hpp"

using namespace std;

MQueueList::MQueueList(size_t max_length) : MQueue::MQueue(max_length) { }

size_t MQueueList::used() const {
    return this->length;
}

bool MQueueList::empty() const {
    return this->length == 0;
}

void MQueueList::insert(const char * data, size_t data_len) {
    this->queue.push(data);
    this->length += data_len;
}

void MQueueList::extract(char * buffer, size_t buffer_len) {
    string & str = this->queue.front();
    buffer[str.copy(buffer, buffer_len - 1)] = '\0';

    this->length -= (str.length() + 1);
    this->queue.pop();
}
