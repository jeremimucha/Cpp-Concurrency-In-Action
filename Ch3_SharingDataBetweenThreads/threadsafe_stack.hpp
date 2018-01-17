#ifndef THREADSAFE_STACK_HPP_
#define THREADSAFE_STACK_HPP_


#include <stdexcept>
#include <memory>
#include <mutex>
#include <stack>


// struct empty_stack : public std::runtime_error
// {
//     using std::runtime_error::runtime_error;
//     const char* what() const noexcept override;
// };

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex mx;
    mutable std::condition_variable data_cond;
public:
    threadsafe_stack() { }
    threadsafe_stack( T&& val )
        : data( std::move(val) )
        { }
    threadsafe_stack( const threadsafe_stack& other )
    {   // note that copy is done within the ctor body to utilize the mutex
        std::lock_guard<std::mutex> lock(other.mx);
        data = other.data;
    }
    threadsafe_stack& operator=( const threadsafe_stack& ) = delete;

    void push(const T& new_value)
    {
        std::lock_guard<std::mutex> lock(mx);
        data.push(new_value);
        data_cond.notify_one();
    }

    void push( T&& new_value )
    {
        std::lock_guard<std::mutex> lock(mx);
        data.push( std::move(new_value) );
        data_cond.notify_one();
    }

    std::shared_ptr<T> pop()
    {
        std::unique_lock<std::mutex> lock(mx);
        // if(data.empty()) throw empty_stack("empty stack");
        data_cond.wait( lock, [this]{return !data.empty();} );
        auto res( std::make_shared<T>(std::move(data.top())) );
        data.pop();
        return res;
    }

    void pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mx);
        // if(data.empty()) throw empty_stack("empty stack");
        data_cond.wait( lock, [this]{return !data.empty();} );
        value = std::move( data.top() );
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mx);
        return data.empty();
    }
};


#endif 
