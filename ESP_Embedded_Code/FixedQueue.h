// FixedSizeQueue.h
#ifndef FIXED_SIZE_QUEUE_H
#define FIXED_SIZE_QUEUE_H

#include <deque>

template <typename T, size_t Capacity>
class FixedSizeQueue
{
public:
    // Adds a new element to the queue.
    void push(const T &value)
    {
        if (queue_.size() >= Capacity)
        {
            queue_.pop_front(); // Remove the oldest element.
        }
        queue_.push_back(value);
    }

    // Removes the oldest element from the queue.
    void pop()
    {
        if (!queue_.empty())
        {
            queue_.pop_front();
        }
    }

    // Accesses the oldest element.
    T &front()
    {
        return queue_.front();
    }

    // Accesses the newest element.
    T &back()
    {
        return queue_.back();
    }

    // Checks if the queue is empty.
    bool empty() const
    {
        return queue_.empty();
    }

    // Returns the current size of the queue.
    size_t size() const
    {
        return queue_.size();
    }

    // Returns the maximum capacity of the queue.
    size_t capacity() const
    {
        return Capacity;
    }

private:
    std::deque<T> queue_;
};

#endif // FIXED_SIZE_QUEUE_H
