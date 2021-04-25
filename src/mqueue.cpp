// Vikman
// April 10, 2021

#include "mqueue.hpp"
#include <cstring>

using namespace std;

MQueue::MQueue(size_t max_length) {
    if (max_length <= 1) {
        throw invalid_argument("Invalid max_length");
    }

    this->length = 0;
    this->max_length = max_length;
}

void MQueue::push(const char * data, MQueue::WaitOpt opt) {
    unique_lock<std::mutex> lock(this->mutex);

    // Check if data would fit

    size_t data_len = strlen(data) + 1;

    if (data_len >= this->max_length - 1) {
        throw NoSpace();
    }

    size_t used = this->used();

    if (opt == MQueue::Wait) {
        while (data_len + used >= this->max_length - 1) {
            cond_popped.wait(lock);
            used = this->used();
        }
    } else {
        if (data_len + used >= this->max_length - 1) {
            throw NoSpace();
        }
    }

    this->insert(data, data_len);
    cond_pushed.notify_all();
}

void MQueue::pop(char * buffer, size_t length, WaitOpt opt) {
    unique_lock<std::mutex> lock(this->mutex);

    // Check if there is data available

    if (opt == MQueue::Wait) {
        while (this->empty()) {
            cond_pushed.wait(lock);
        }
    } else {
        if (this->empty()) {
            throw NoData();
        }
    }

    this->extract(buffer, length);
    cond_popped.notify_all();
}

const char * MQueue::NoSpace::what() const noexcept {
    return "No suitable space";
}

const char * MQueue::NoData::what() const noexcept {
    return "No data available";
}
