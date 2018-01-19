#ifndef FIND_ASYNC_HPP_
#define FIND_ASYNC_HPP_

#include <algorithm>
#include <future>


template<typename Iterator, typename MatchType>
Iterator parallel_find_impl(Iterator first, Iterator last, MatchType match,
                            std::atomic<bool>& done)
{
    try{
        const unsigned long length = std::distance(first, last);
        const unsigned long min_per_thread = 25;
        if( length<(2*min_per_thread) ){
            for(; (first!=last) && !done.load(); ++first){
                if( *first == match ){
                    done.store(true);
                    return first;
                }
            }
            return last;
        }
        else{
            const Iterator mid_point = first;
            std::advance( const_cast<Iterator>(mid_point), length/2 );
            std::future<Iterator> async_result =
                std::async(&parallel_find_impl<Iterator, MatchType>,
                           mid_point, last, match, std::ref(done));
            const Iterator direct_result =
                parallel_find_impl(first, mid_point, match, done);
            return (direct_result == mid_point) ?
                async_result.get() : direct_result;
        }
    }
    catch(...){
        done.store(true);
        throw;
    }
}

template<typename Iterator, typename MatchType>
Iterator parallel_find( Iterator first, Iterator last, MatchType match )
{
    std::atomic<bool> done(false);
    return parallel_find_impl(first, last, match, done);
}


#endif /* FIND_ASYNC_HPP_ */
