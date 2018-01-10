#ifndef SIMPLE_QUEUE_HPP_
#define SIMPLE_QUEUE_HPP_

#include <memory>
#include <utility>


template<typename T>
class Queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::unique_ptr<node> head;
    node* tail;
public:
    Queue()
        : head(new node), tail(head.get())
        { }
    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;

    std::shared_ptr<T> try_pop()
    {
        if(head.get() == tail){
            return nullptr;
        }
        const auto res(head->data);
        auto old_head( std::move(head) );
        head = std::move( old_head->next );
        return res;
    }

    void push( T new_value )
    {
        auto new_data( std::make_shared<T>(std::move(new_value)) );
        auto p( std::make_unique<node>() );
        tail->data = new_data;
        node* const new_tail = p.get();
        tail->next = std::move(p);
        tail = new_tail;
    }

    template<typename... Args>
    void emplace( Args&&... args )
    {
        auto new_data( std::make_shared<T>( std::forward<Args>(args)... ) );
        auto p( std::make_unique<node>() );
        tail->data = new_data;
        node* const new_tail = p.get();
        tail->next = std::move(p);
        tail = new_tail;
    }
};


#endif /* SIMPLE_QUEUE_HPP_ */
