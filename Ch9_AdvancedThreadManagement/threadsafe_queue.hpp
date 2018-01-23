#ifndef THREADSAFE_QUEUE_HPP_
#define THREADSAFE_QUEUE_HPP_

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;

public:
    threadsafe_queue()
        : head(new node), tail(head.get())
        { }
    threadsafe_queue( const threadsafe_queue& ) = delete;
    threadsafe_queue& operator=( const threadsafe_queue& ) = delete;

    std::shared_ptr<T> try_pop();
    bool try_pop( T& value );
    std::shared_ptr<T> wait_and_pop();
    void wait_and_pop( T& value );
    void push(T new_value);
    template<typename... Args>
    void emplace( Args&&... args );
    bool empty();

private:
    node* get_tail();
    std::unique_ptr<node> pop_head();
    std::unique_lock<std::mutex> wait_for_data();
    std::unique_ptr<node> wait_pop_head();
    std::unique_ptr<node> wait_pop_head(T& value);
    std::unique_ptr<node> try_pop_head();
    std::unique_ptr<node> try_pop_head(T& value);
};


template<typename T>
void threadsafe_queue<T>::push(T new_value)
{
    auto new_data( std::make_shared<T>(std::move(new_value)) );
    auto p( std::make_unique<node>() );
    { std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        node* const new_tail( p.get() );
        tail->next = std::move(p);
        tail = new_tail;
    }
    data_cond.notify_one();
}

template<typename T>
    template<typename... Args>
void threadsafe_queue<T>::emplace( Args&&... args )
{
    auto new_data( std::make_shared<T>( std::forward<Args>(args)... ) );
    auto p( std::make_unique<node>() );
    { std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        node* const new_tail( p.get() );
        tail->next = std::move(p);
        tail = new_tail;
    }
    data_cond.notify_one();
}

template<typename T>
typename threadsafe_queue<T>::node* threadsafe_queue<T>::get_tail()
{
    std::lock_guard<std::mutex> tail_lock(tail_mutex);
    return tail;
}
template<typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::pop_head()
{
    auto old_head( std::move(head) );
    head = std::move( old_head->next );
    return old_head;
}
template<typename T>
std::unique_lock<std::mutex> threadsafe_queue<T>::wait_for_data()
{
    std::unique_lock<std::mutex> head_lock(head_mutex);
    data_cond.wait( head_lock, [this]{ return head.get() != get_tail(); } );
    return std::move(head_lock);
}
template<typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::wait_pop_head()
{
    std::unique_ptr<std::mutex> head_lock(wait_for_data());
    return pop_head();
}
template<typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::wait_pop_head(T& value)
{
    std::unique_lock<std::mutex> head_lock(wait_for_data());
    value = std::move( *head->data );
    return pop_head();
}


template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop()
{
    const auto old_head( wait_pop_head() );
    return old_head->data;
}

template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value)
{
    const auto old_head( wait_pop_head(value) );
}

template<typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::try_pop_head()
{
    std::lock_guard<std::mutex> head_lock(head_mutex);
    if(head.get() == get_tail()){
        return nullptr;
    }
    return pop_head();
}

template<typename T>
std::unique_ptr<typename threadsafe_queue<T>::node>
threadsafe_queue<T>::try_pop_head(T& value)
{
    std::lock_guard<std::mutex> head_lock(head_mutex);
    if(head.get() == get_tail()){
        return nullptr;
    }
    value = std::move( *head->data );
    return pop_head();
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop()
{
    auto old_head( try_pop_head() );
    return old_head ? old_head->data : nullptr;
}

template<typename T>
bool threadsafe_queue<T>::try_pop(T& value)
{
    const auto old_head( try_pop_head(value) );
    return old_head != nullptr;
}

template<typename T>
bool threadsafe_queue<T>::empty()
{
    std::lock_guard<std::mutex> head_lock( head_mutex );
    return ( head.get() == get_tail() );
}



#endif 
