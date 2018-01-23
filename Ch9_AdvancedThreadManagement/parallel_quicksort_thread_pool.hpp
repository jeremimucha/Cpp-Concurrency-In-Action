#ifndef PARALLEL_QUICKSORT_THREAD_POOL_HPP_
#define PARALLEL_QUICKSORT_THREAD_POOL_HPP_

#include <algorithm>
#include <list>
#include "waitable_thread_pool.hpp"


template<typename T>
struct sorter
{
    thread_pool pool;
    std::list<T> do_sort(std::list<T>& data_chunk)
    {
        if(data_chunk.empty()){
            return data_chunk;
        }

        std::list<T> result;
        result.splice(result.begin(), data_chunk, data_chunk.begin());
        const T& partition_val = result.front();
        typename std::list<T>::iterator divide_point =
            std::partition(data_chunk.begin(), data_chunk.end(),
                [&partition_val](const T& val){ return val < partition_val; });
        
        std::list<T> new_lower_chunk;
        new_lower_chunk.splice(new_lower_chunk.end(),
                               data_chunk, data_chunk.begin(),
                               divide_point);

        std::future<std::list<T>> new_lower =
            pool.submit([this, new_lower_chunk=std::move(new_lower_chunk)]
                        {do_sort(new_lower_chunk);});
        
        std::list<T> new_higher(do_sort(data_chunk));
        result.splice(result.end(), new_higher);
        while(!new_lower.wait_for(std::chrono::second(0)) ==
                std::future_status::timeout){
            pool.run_pending_task();
        }
        result.splice(result.begin(), new_lower.get());
        return result;
    }
};

template<typename T>
std::list<T> parallel_quicksort(std::list<T> input)
{
    if(input.empty()){
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);
}

#endif /* PARALLEL_QUICKSORT_THREAD_POOL_HPP_ */
