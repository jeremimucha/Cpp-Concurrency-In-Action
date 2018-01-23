#ifndef PARALLEL_ACCUMULATE_WAITABLE_POOL_HPP_
#define PARALLEL_ACCUMULATE_WAITABLE_POOL_HPP_

#include <algorithm>
#include <vector>
#include <future>
#include "waitable_thread_pool.hpp"


template<typename Iterator, typename T>
struct accumulate_block
{
    T operator()( Iterator first, Iterator last )
    {
        return std::accumulate(first, last, T());
    }
};

template<typename Iterator, typename T>
T parallel_accumulate( Iterator first, Iterator last, T init )
{
    const unsigned long length = std::distance(first, last);

    if(!length)
        return init;

    const unsigned long block_size = 25;
    const unsigned long num_blocks = (length+block_size-1)/block_size;

    std::vector<std::future<T>> futures; futures.reserve(num_blocks-1);
    thread_pool pool;

    Iterator block_start = first;
    for(unsigned long i=0; i<(num_blocks-1); ++i){
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        futures.push_back(accumulate_block<Iterator,T>());
        block_start = block_end;
    }

    T last_result = accumulate_block<Iterator,T>()(block_start, last);
    T result = init;
    for(unsigned long i=0; i<(num_blocks-1); ++i){
        result += futures[i].get();
    }
    result += last_result;
    return result;
}


#endif /* PARALLEL_ACCUMULATE_WAITABLE_POOL_HPP_ */
