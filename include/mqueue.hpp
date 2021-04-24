// Vikman
// April 9, 2021

#ifndef MQUEUE_H
#define MQUEUE_H

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

class MQueue {
public:
    class NoSpace : public std::exception {
    public:
        const char * what() const noexcept;
    };

    class NoData : public std::exception {
    public:
        const char * what() const noexcept;
    };

    enum ShrinkOpt {
        NoShrink,
        Shrink
    };

    enum WaitOpt {
        NoWait,
        Wait
    };

    MQueue(size_t max_length, ShrinkOpt opt = NoShrink);
    ~MQueue();

    void push(const char * data, WaitOpt opt = NoWait);
    void pop(char * buffer, size_t length, WaitOpt opt = NoWait);

    size_t used() const;
    bool empty() const;

private:
    char * memory;
    char * head;
    char * tail;
    size_t length;
    size_t max_length;
    ShrinkOpt shrink_opt;
    std::mutex mutex;
    std::condition_variable cond_pushed;
    std::condition_variable cond_popped;

    void expand(size_t new_length);
    void shrink(size_t new_length);
    void insert(const char * data, size_t data_len);
    void extract(char * buffer, size_t buffer_len);
    void trim();
};

#endif
