// using RAII to wait for a thread to complete
#ifndef THREAD_GUARD_H_
#define THREAD_GUARD_H_

#include <thread>


class thread_guard
{
    std::thread& t;
public:
    explicit thread_guard( std::thread& t_)
        : t(t_) { }
    ~thread_guard()
    {
        if(t.joinable())
            t.join();
    }
    thread_guard(const thread_guard&) = delete;
    thread_guard& operator=(const thread_guard&) = delete;
};

#endif
