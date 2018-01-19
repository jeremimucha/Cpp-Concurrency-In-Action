#ifndef FOR_EACH_ASYNC_HPP_
#define FOR_EACH_ASYNC_HPP_

#include <algorithm>
#include <future>


template<typename Iterator, typename Function>
void parallel_for_each(Iterator first, Iterator last, Function func)
{
    const unsigned long length = std::distance(first, last);
    if(!length)
        return;

    const unsigned long min_per_thread = 25;

    if(length < (2*min_per_thread)){
        std::for_each(first, last, func);
    }
    else{
        const Iterator mid_point = first;
        std::advance( const_cast<Iterator>(mid_point), length/2 );
        
        std::future<void> first_half =
            std::async(&parallel_for_each<Iterator,Function>,
                first, mid_point, func);
        parallel_for_each(mid_point, last, func);
        first_half.get();
    }
}


#endif /* FOR_EACH_ASYNC_HPP_ */
