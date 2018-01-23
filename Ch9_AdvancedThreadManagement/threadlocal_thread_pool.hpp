#ifndef THREADLOCAL_THREAD_POOL_HPP_
#define THREADLOCAL_THREAD_POOL_HPP_

#include <thread>
#include <atomic>
#include <future>
#include <queue>
#include <memory>
#include <utility>
#include <type_traits>
#include "function_wrapper.hpp"
#include "threadsafe_queue.hpp"
#include "join_threads.hpp"


class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> pool_work_queue;
    using local_queue_type = std::queue<function_wrapper>;
    static thread_local std::unique_ptr<local_queue_type> local_work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;


    void worker_thread()
    {
        local_work_queue.reset(new local_queue_type);
        while(!done){
            run_pending_task();
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
    std::future<std::result_of_t<FunctionType()>>
    submit(FunctionType f)
    {
        using result_type = std::result_of_t<FunctionType()>;

        std::packaged_task<result_type()> taks(f);
        std::future<result_type> res(task.get_future());
        if(local_work_queue){
            local_work_queue->push(std::move(task));
        }
        else{
            pool_work_queue.push(std::move(task));
        }
        return res;
    }

    void run_pending_task()
    {
        function_wrapper task;
        if(local_work_queue && !local_work_queue->empty()){
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        }
        else if( pool_work_queue.try_pop(task) ){
            task();
        }
        else{
            std::this_thread::yield();
        }
    }
};


#endif /* THREADLOCAL_THREAD_POOL_HPP_ */
