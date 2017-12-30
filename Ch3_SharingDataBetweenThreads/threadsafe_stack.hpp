#ifndef THREADSAFE_STACK_HPP_
#define THREADSAFE_STACK_HPP_


#include <stdexcept>
#include <memory>
#include <mutex>
#include <stack>


struct empty_stack : std::runtime_error
{
    const char* what() const noexcept;
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex mx;
public:
    threadsafe_stack() { }
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
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(mx);
        if(data.empty()) throw empty_stack();
        auto const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mx);
        if(data.empty()) throw empty_stack();
        value = data.top();
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mx);
        return data.empty();
    }
};


#endif 
