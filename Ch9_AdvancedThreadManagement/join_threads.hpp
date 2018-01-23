#ifndef JOIN_THREADS_
#define JOIN_THREADS_

#include <vector>
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


#endif /* JOIN_THREADS_ */
