#ifndef THEADSAFE_LIST_HPP_
#define THEADSAFE_LIST_HPP_

#include <mutex>
#include <memory>
#include <utility>
#include <iostream>


template<typename T>
class threadsafe_list
{
private:

    struct debug_delete
    {
        template<typename U>
        void operator()( U* obj ) noexcept
        {
            std::cerr << "Deleting object " << reinterpret_cast<void*>(obj)
                << " value: " << *obj->data << std::endl;
            delete obj;
        }
    };

    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node,debug_delete> next;

        node() : next(nullptr,debug_delete()) { }
        node( const T& value )
            : data(std::make_shared<T>(value))
            { }
    };

// --- member variables
    node head;
// ---

public:

    threadsafe_list() = default;
    ~threadsafe_list()
    {
        remove_if( [](const node&){ return true; } );
    }

    threadsafe_list(const threadsafe_list&) = delete;
    threadsafe_list& operator=(const threadsafe_list&) = delete;

    void push_front( const T& value )
    {
        std::unique_ptr<node,debug_delete> new_node(new node(value), debug_delete());
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::move(head.next);
        head.next = std::move(new_node);
    }

    template<typename Function>
    void for_each( Function func )
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while( node* const next = current->next.get() ){
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();    // we unlock this as soon as possible
            func(*next->data);
            current = next;
            lk = std::move(next_lk);
        }
    }

    template<typename Predicate>
    std::shared_ptr<T> find_first_if( Predicate pred )
    {
        node* current = &head;
    // need to lock the head before we access the node it points to
        std::unique_lock<std::mutex> lk(head.m);
        while( node* const next = current->next.get() ){
        // lock the next node before accessing the data it points to
            std::unique_lock<std::mutex> next_lk(next->m);
        // unlock this mutex before calling user-supplied code
            lk.unlock();
            if( pred(*next->data) ){
                return next->data;
            }
            current = next;
        // transfer lock ownership so that we keep the (now) current node locked
            lk = std::move(next_lk);
        }
        return nullptr;
    }

    template<typename Predicate>
    void remove_if( Predicate p )
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while(node* const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk( next->m );
            if( p(*next->data) ){
            // transfer ownership into this scope - old_next will be destroyed
            // on scope exit
                std::unique_ptr<node,debug_delete> old_next = std::move( current->next );
                current->next = std::move( next->next );
            // need to unlock this before old_next is destroyed
            // destroying a locked mutex is undefined behavior
                next_lk.unlock();
            }
            else{
            // otherwise just iterate
                lk.unlock();
                current = next;
                lk = std::move(next_lk);
            }
        }
    }

};


#endif /* THEADSAFE_LIST_HPP_ */
