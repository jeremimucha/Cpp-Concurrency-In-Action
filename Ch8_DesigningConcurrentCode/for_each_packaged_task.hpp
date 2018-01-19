#ifndef FOR_EACH_PACKAGED_TASK_HPP_
#define FOR_EACH_PACKAGED_TASK_HPP_

#include <algorithm>
#include <vector>
#include <future>
#include <thread>
#include "join_threads.hpp"


template<typename Iterator, typename Function>
void parallel_for_each(Iterator first, Iterator last, Function func)
{
    const unsigned long length = std::distance(first, last);
    if(!length)
        return;

    const unsigned long min_per_thread = 25;
    const unsigned long max_threads =
        (length + min_per_thread - 1)/min_per_thread;

    const unsigned long hardware_threads = std::thread::hardware_concurrency();
    const unsigned long num_threads =
        std::min((hardware_threads != 0 ? hardware_threads : 2), max_threads);
    
    const unsigned long block_size = length/num_threads;

    std::vector<std::future<void>> futures;
    futures.reserve(num_threads-1);
    std::vector<std::thread> threads;
    threads.reserve(num_threads-1);
    join_threads joiner(threads);

    Iterator block_start = first;
    for(unsigned long i=0; i<(num_threads-1); ++i){
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<void(void)> task(
            [block_start, block_end, func](){
                std::for_each(block_start, block_end, func);
            });
        futures.push_back( task.get_future() );
        threads.push_back( std::thread(std::move(task)) );
        block_start = block_end;
    }
    std::for_each(block_start, last, func);
    for(unsigned long i=0; i<(num_threads-1); ++i){
        futures[i].get();   // strictly for the purpose of propagating exceptions
    }
}


#endif /* FOR_EACH_PACKAGED_TASK_HPP_ */
