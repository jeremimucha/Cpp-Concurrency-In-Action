#ifndef THREADSAFE_QUEUE_HPP_
#define THREADSAFE_QUEUE_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>


template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mtx;
    std::condition_variable data_cond;
    std::queue<T> data_queue;
public:
    threadsafe_queue() = default;
    threadsafe_queue( const threadsafe_queue& other )
    {
        std::lock_guard<std::mutex> lk(other.mtx);
        data_queue = other.data_queue();
    }

    template<typename U>
    void push( U&& new_value )
    {
        std::lock_guard<std::mutex> lk(mtx);
        data_queue.push(std::forward<U>(new_value));
        data_cond.notify_one();
    }

    void wait_and_pop( T& value )
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        auto res( std::make_shared<T>(std::move(data_queue.front())) );
        data_queue.pop();
        return res;
    }

    bool try_pop( T& value )
    {
        std::lock_guard<std::mutex> lk(mtx);
        if( data_queue.empty() )
            return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mtx);
        if( data_queue.empty() )
            return nullptr;
        auto res( std::make_shared<T>(std::move(data_queue.front())) );
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mtx);
        return data_queue.empty();
    }
};


#endif
