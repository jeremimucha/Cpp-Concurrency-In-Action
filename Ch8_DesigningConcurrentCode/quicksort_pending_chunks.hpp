#ifndef QUICKSORT_PENDING_CHUNKS_
#define QUICKSORT_PENDING_CHUNKS_

#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <algorithm>
#include <atomic>
#include "threadsafe_stack.hpp"


template<typename T>
struct sorter
{
    struct chunk_to_sort
    {
        std::list<T> data;
        std::promise<std::list<T>> promise;
    };

// --- member variables
    threadsafe_stack<chunk_to_sort> chunks;
    std::vector<std::thread> threads;
    const unsigned max_thread_count;
    std::atomic<bool> end_of_data;
// ---

    sorter()
        : max_thread_count( std::thread::hardware_concurrency()-1 )
        , end_of_data( false )
        { }
    
    ~sorter()
    {
        end_of_data = true;
        for(unsigned i=0; i<threads.size(); ++i){
            threads[i].join();
        }
        // for( auto it = threads.begin(); it!=threads.end(); ++it){
        //     it->join();
        // }
    }

    void try_sort_chunk()
    {
        std::shared_ptr<chunk_to_sort> chunk( chunks.wait_and_pop() );
        if( chunk ){
            sort_chunk(chunk);
        }
    }

    std::list<T> do_sort(std::list<T>& chunk_data)
    {
        if(chunk_data.empty()){
            return chunk_data;
        }

        std::list<T> result;
        result.splice( result.begin(), chunk_data, chunk_data.begin() );
        const T& partition_val = result.front();

        auto div_point = std::partition( chunk_data.begin(), chunk_data.end()
                                       , [&partition_val](const T& val)
                                         {return val < partition_val;}
                                       );
        
        chunk_to_sort new_lower_chunk;
        new_lower_chunk.data.splice( new_lower_chunk.data.end()
                                   , chunk_data
                                   , chunk_data.begin(), div_point
                                   );
        std::future<std::list<T>> new_lower = new_lower_chunk.promise.get_future();
        chunks.push(std::move(new_lower_chunk));
        if( threads.size() < max_thread_count ){
            threads.push_back( std::thread(&sorter<T>::sort_thread, this) );
        }

        std::list<T> new_higher(do_sort(chunk_data));

        result.splice(result.end(), new_higher);
        while( new_lower.wait_for(std::chrono::seconds(0)) !=
               std::future_status::ready ){
            try_sort_chunk();
        }
        result.splice( result.begin(), new_lower.get() );
        
        return result;
    }

    void sort_chunk( const std::shared_ptr<chunk_to_sort>& chunk )
    {
        chunk->promise.set_value(do_sort(chunk->data));
    }

    void sort_thread()
    {
        std::cerr << "thread[" << std::this_thread::get_id()
            << "] started." << std::endl;
        while(!end_of_data){
            try_sort_chunk();
            std::this_thread::yield();
        }
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


#endif /* QUICKSORT_PENDING_CHUNKS_ */
