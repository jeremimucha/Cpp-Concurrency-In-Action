#ifndef WAITABLE_THREAD_POOL_HPP
#define WAITABLE_THREAD_POOL_HPP

#include <thread>
#include <atomic>
#include <future>
#include <vector>
#include <type_traits>
#include "threadsafe_queue.hpp"
#include "function_wrapper.hpp"
#include "join_threads.hpp"


class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

// --- modified code
    void worker_thread()
    {
        while(!done){
            function_wrapper task;
            if(work_queue.try_pop(task)){
                task();
            }
            else{
                std::this_thread::yield();
            }
        }
    }
// --- modified code

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

// --- modified code
    template<typename FunctionType>
    std::future<std::result_of_t<FunctionType()>>
    submit(FunctionType f)
    {
        using result_type = std::result_of_t<FunctionType()>;

        std::packaged_task<reslt_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
// --- modified code

// --- modified code
// avoid deadlocks by allowing running tasks synchronously while waiting for
// pending tasks to finish
    void run_pending_task()
    {
        function_wrapper task;
        if(work_queue.try_pop(task)){
            task();
        }
        else{
            std::this_thread::yield();
        }
    }
// --- modified code
};


#endif /* WAITABLE_THREAD_POOL_HPP */
