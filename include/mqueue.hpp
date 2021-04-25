// Vikman
// April 9, 2021

#ifndef MQUEUE_H
#define MQUEUE_H

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

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

    enum WaitOpt {
        NoWait,
        Wait
    };

    MQueue(size_t max_length);
    virtual ~MQueue() { }

    void push(const char * data, WaitOpt opt = NoWait);
    void pop(char * buffer, size_t length, WaitOpt opt = NoWait);

    virtual size_t used() const = 0;
    virtual bool empty() const = 0;

protected:
    virtual void insert(const char * data, size_t data_len) = 0;
    virtual void extract(char * buffer, size_t buffer_len) = 0;

    size_t length;
    size_t max_length;
    std::mutex mutex;
    std::condition_variable cond_pushed;
    std::condition_variable cond_popped;
};

class MQueueRing : public MQueue {
public:
    enum ShrinkOpt {
        NoShrink,
        Shrink
    };

    MQueueRing(size_t max_length, ShrinkOpt opt = NoShrink);
    ~MQueueRing();

    size_t used() const;
    bool empty() const;

protected:
    void insert(const char * data, size_t data_len);
    void extract(char * buffer, size_t buffer_len);

private:
    void expand(size_t new_length);
    void shrink(size_t new_length);
    void trim();

    char * memory;
    char * head;
    char * tail;
    ShrinkOpt shrink_opt;
};

class MQueueList : public MQueue {
public:
    MQueueList(size_t max_length);

    size_t used() const;
    bool empty() const;

protected:
    void insert(const char * data, size_t data_len);
    void extract(char * buffer, size_t buffer_len);

private:
    std::queue<std::string> queue;
};

#endif
