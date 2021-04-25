// Vikman
// April 25, 2021

#include "mqueue.hpp"
#include <cstring>

using namespace std;

MQueueRing::MQueueRing(size_t max_length, MQueueRing::ShrinkOpt opt) : MQueue::MQueue(max_length) {
    this-> memory = NULL;
    this->head = NULL;
    this->tail = NULL;
    this->shrink_opt = opt;
}

MQueueRing::~MQueueRing() {
    free(this->memory);
}

size_t MQueueRing::used() const {
    return this->length > 0 ? (this->tail + this->length - this->head) % this->length : 0;
}

bool MQueueRing::empty() const {
    return this->head == this->tail;
}

void MQueueRing::insert(const char * data, size_t data_len) {
    // Check if data currently fits
    size_t used = this->used();

    if (data_len + used >= this->length) {
        this->expand(data_len + used + 1);
    }

    size_t tail_len = this->tail - this->memory;

    if (tail_len + data_len <= this->length) {
        memcpy(this->tail, data, data_len);
    } else {
        size_t chunk_len = this->length - tail_len;
        memcpy(this->tail, data, chunk_len);
        memcpy(this->memory, data + chunk_len, data_len - chunk_len);
    }

    this->tail = this->memory + (tail_len + data_len) % this->length;
}

void MQueueRing::extract(char * buffer, size_t buffer_len) {
    size_t max_len = this->length - (this->head - this->memory);
    size_t chunk_len = strnlen(this->head, max_len);

    if (chunk_len < max_len) {
        chunk_len++;

        if (chunk_len > buffer_len) {
            chunk_len = buffer_len;
        }

        memcpy(buffer, this->head, chunk_len);
        this->head = this->memory + (this->head - this->memory + chunk_len) % this->length;
    } else {
        if (chunk_len > buffer_len) {
            chunk_len = buffer_len;
        }

        memcpy(buffer, this->head, chunk_len);

        if (chunk_len < buffer_len) {
            strncpy(buffer + chunk_len, this->memory, buffer_len - chunk_len);
        }

        this->head = this->memory + strlen(this->memory) + 1;
    }

    buffer[buffer_len - 1] = '\0';

    if (this->empty()) {
        this->trim();
    } else if (this->shrink_opt & MQueueRing::Shrink && this->used() < this->length / 2) {
        this->shrink(this->length / 2);
    }
}

void MQueueRing::expand(size_t new_length) {
    char * new_memory = (char *)realloc(this->memory, new_length);
    if (new_memory == NULL) {
        throw runtime_error("Cannot allocate memory");
    }

    this->head += new_memory - this->memory;
    this->tail += new_memory - this->memory;
    this->memory = new_memory;

    if (this->tail < this->head) {
        size_t tail_len = this->tail - this->memory;
        size_t growth = new_length - this->length;

        if (tail_len <= growth) {
            memcpy(this->memory + this->length, this->memory, tail_len);
            this->tail = this->memory + (this->length + tail_len) % new_length;
        } else {
            memcpy(this->memory + this->length, this->memory, growth);
            memmove(this->memory, this->memory + growth, tail_len - growth);
            this->tail -= growth;
        }
    }

    this->length = new_length;
}

void MQueueRing::shrink(size_t new_length) {
    if (this->empty()) {
        this->head = this->tail = this->memory;
    } else {
        size_t head_len = this->head - this->memory;
        size_t tail_len = this->tail - this->memory;

        if (head_len < tail_len) {
            // Data is contiguous
            if (new_length <= head_len) {
                // Chunk is fully at right. Move it to the memory beginning.
                memcpy(this->memory, this->head, this->tail - this->head);
                this->head = this->memory;
                this->tail -= head_len;
            } else if (new_length <= tail_len) {
                // Chunk needs to be splitted
                size_t chunk_len = tail_len - new_length;
                memcpy(this->memory, this->tail - chunk_len, chunk_len);
                this->tail = this->memory + chunk_len;
            }

            // Otherwise, the chunk is fully at left. Do nothing.
        } else {
            // Data is already splitted. Move the head to the left.
            char * new_head = this->head - (this->length - new_length);

            if (head_len < new_length) {
                // Strings may overlap
                memmove(new_head, this->head, this->length - head_len);
            } else {
                memcpy(new_head, this->head, this->length - head_len);
            }

            this->head = new_head;
        }
    }

    char * new_memory = (char *)realloc(this->memory, new_length);
    if (new_memory == NULL) {
        throw runtime_error("Cannot allocate memory");
    }

    this->head += new_memory - this->memory;
    this->tail += new_memory - this->memory;
    this->memory = new_memory;
    this->length = new_length;
}

void MQueueRing::trim() {
    free(this->memory);
    this->memory = this->head = this->tail = NULL;
    this->length = 0;
}
