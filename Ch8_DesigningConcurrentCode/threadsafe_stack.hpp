#ifndef THREADSAFE_STACK_HPP_
#define THREADSAFE_STACK_HPP_

#include <mutex>
#include <condition_variable>
#include <memory>
#include <utility>


template<typename T>
class threadsafe_stack
{
private:
    struct node
    {
        std::unique_ptr<T> data;
        std::unique_ptr<node> next;

        node() = default;
        template<typename... Args>
        node( Args&&... args )
            : data( std::make_unique<T>(std::forward<Args>(args)...) )
            { }
    };

// --- member variables
    mutable std::mutex head_mutex;
    mutable std::condition_variable data_cond;
    node head;
// ---

    std::unique_ptr<T> pop_head()
    {}

public:
    threadsafe_stack() = default;

    template<typename U>
    void push( U&& value )
    {
        auto new_node( std::make_unique<node>(std::forward<U>(value)) );
        { std::lock_guard<std::mutex> lk(head_mutex);
            new_node->next = std::move(head.next);
            head.next = std::move(new_node);
        }
        data_cond.notify_one();
    }

    std::unique_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(head_mutex);
        data_cond.wait( lk, [this]{return head.next != nullptr;} );
        auto old_head = std::move(head.next);
        head.next = std::move(old_head->next);
        return std::move(old_head->data);
    }

    void wait_and_pop( T& value )
    {
        std::unique_lock<std::mutex> lk(head_mutex);
        data_cond.wait( lk, [this]{return head.next != nullptr;} );
        auto old_head = std::move(head.next);
        head.next = std::move(old_head->next);
        lk.unlock();
        value = *old_head->data;
    }

    std::unique_ptr<T> try_pop()
    {
        if( empty() ){
            return nullptr;
        }
        std::lock_guard<std::mutex> lk(head_mutex);
        auto old_head = std::move( head.next );
        head.next = std::move( old_head->next );
        return std::move(old_head->data);
    }

    bool try_pop( T& value )
    {
        if( empty() ){
            return nullptr;
        }
        std::lock_guard<std::mutex> lk(head_mutex);
        auto old_head = std::move( head.next );
        head.next = std::move( old_head->next );
        value = *old_head->data;
    }

    bool empty() const noexcept
    {
        return head.next == nullptr;
    }
};


#endif /* THREADSAFE_STACK_HPP_ */
