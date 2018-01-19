#ifndef ACCUMULATE_ASYNC_
#define ACCUMULATE_ASYNC_

/* 
** This is a thread-safe and exception-safe implementation of a concurrent,
** recursive accumulate algorithm using std::async and std::future.
** The exception safety is provided by the futures here. If an exception is
** thrown by the recursive call the std::future created from the call
** to std::async will be destroyed as the exception propagates. This will
** in turn wait for the asynchronous task to finish thus avoiding a dangling
** thread.
** This implementation makes use of the standard library to handle the division
** of work. The library ensures that std::async calls make use of the hardware
** threads that are available without creating an unnecesserily large number
** of threads. Some of the "asynchronous" calls will actually be executed
** synchronously at the call to .get().
*/


#include <algorithm>
#include <numeric>
#include <future>


template<typename Iterator, typename T>
T parallel_accumulate( Iterator first, Iterator last, T init )
{
    const unsigned length = std::distance(first, last);
    const unsigned max_chunk_size = 25;
    if(length <= max_chunk_size){
        return std::accumulate(first, last, init);
    }
    
    Iterator mid_point = first;
    std::advance( mid_point, length/2 );
    std::future<T> first_half_result =
        std::async( parallel_accumulate<Iterator,T>
                  , first, mid_point, init
                  );
    
    T second_half_result = parallel_accumulate(mid_point, last, T());

    return first_half_result.get() + second_half_result;
}


#endif /* ACCUMULATE_ASYNC_ */
