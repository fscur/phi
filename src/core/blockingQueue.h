#pragma once
#include <phi.h>

namespace phi
{
    template <typename T>
    class blockingQueue
    {
    private:
        std::mutex _mutex;
        std::queue<T> _queue;
        std::condition_variable _isNotEmpty;
        inline bool queueIsNotEmpty() { return !_queue.empty(); };

    public:

        T front()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _isNotEmpty.wait(lock, std::bind(&blockingQueue::queueIsNotEmpty, this));
            return _queue.front();
        }

        void pop()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _isNotEmpty.wait(lock, std::bind(&blockingQueue::queueIsNotEmpty, this));
            _queue.pop();
        }

        void push(const T& value)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _queue.push(value);
            _isNotEmpty.notify_one();
        }

        void push(T&& value)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _queue.push(value);
            _isNotEmpty.notify_one();
        }

        bool empty()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _queue.empty();
        }

        size_t size()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _queue.size();
        }

        T back()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _isNotEmpty.wait(lock, std::bind(&blockingQueue::queueIsNotEmpty, this));
            return _queue.back();
        }

        void emplace(T value)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _queue.emplace(value);
            _isNotEmpty.notify_one();
        }
    };
}