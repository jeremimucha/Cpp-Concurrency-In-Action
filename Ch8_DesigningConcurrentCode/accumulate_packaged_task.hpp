#ifndef ACCUMULATE_PACKAGED_TASK_
#define ACCUMULATE_PACKAGED_TASK_

/* 
** This is a thread-safe and exception-safe implementation of a concurrent
** accumulate algorithm using std::packaged_task, std::future and std::thread.
** std::packaged_task in combination with the join_threads class provides
** exception safety. All the exceptions are caught and propagated by
** std::packaged_task, while the join_threads class assures that if any exceptions
** are thrown all the threads are .join()'ed and closed correctly.
*/

#include <algorithm>
#include <numeric>
#include <vector>
#include <future>
#include <thread>


class join_threads
{
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_)
        : threads( threads_ )
        { }
    ~join_threads()
    {
        for(auto it = threads.begin(); it != threads.end(); ++it){
            if( it->joinable() )
                it->join();
        }
    }
};

template<typename Iterator, typename T>
struct accumulate_block
{
    T operator()( Iterator first, Iterator last )
    {
        return std::accumulate(first, last, T());
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    const unsigned long length = std::distance(first, last);

    if(!length)
        return init;

    const unsigned long min_per_thread = 25;
    const unsigned long max_threads = (length + min_per_thread - 1) / min_per_thread;

    const unsigned long hardware_threads = std::thread::hardware_concurrency();
    const unsigned long num_threads =
        std::min( (hardware_threads!=0 ? hardware_threads : 2), max_threads );
    
    const unsigned long block_size = length / num_threads;

    std::vector<std::future<T>> futures;
        futures.reserve(num_threads-1);
    std::vector<std::thread> threads;
        threads.reserve(num_threads-1);
    // object similiar to scoped_thread - make sure all the threads are
    // .join()'ed on scope exit - this provides exception safety
    // ensuring that all threads are .joined and destoyed correctly
    // on all scope exit paths
    join_threads joiner(threads);

    Iterator block_start = first;
    for(unsigned long i=0; i<(num_threads-1); ++i){
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<T(Iterator,Iterator)> task(accumulate_block<Iterator,T>());
        futures.push_back( task.get_future() );
        threads.push_back( std::thread(std::move(task), block_start, block_end) );
        block_start = block_end;
    }

    T last_result = accumulate_block<Iterator,T>()(block_start, last);
    T result = init;
    for(auto it = futures.cbegin(); it!=futures.cend(); ++it){
        result += it->get();
    }
    result += last_result;
    return result;
}


#endif /* ACCUMULATE_PACKAGED_TASK_ */
