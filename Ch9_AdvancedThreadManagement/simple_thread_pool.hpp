#ifndef SIMPLE_THREAD_POOL_HPP_
#define SIMPLE_THREAD_POOL_HPP_

#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include "threadsafe_queue.hpp"
#include "join_threads.hpp"


class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while(!done){
            std::function<void()> task;
            if(work_queue.try_pop(task)){
                task();
            }
            else{
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool()
        : done(false), joiner(threads)
    {
        const unsigned thread_count = std::thread::hardware_concurrency();
        try{
            for(unsigned i=0; i<thread_count; ++i){
                threads.push_back(
                    std::thread(&thread_pool::worker_thread,this) );
            }
        }
        catch(...){
            done = true;
            throw;
        }
    }

    ~thread_pool()
    {
        done = true;
    }

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        work_queue.push(std::function<void()>(f));
    }
};


#endif /* SIMPLE_THREAD_POOL_HPP_ */
