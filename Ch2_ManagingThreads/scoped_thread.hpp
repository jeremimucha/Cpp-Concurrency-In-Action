#ifndef SCOPED_THREAD_H_
#define SCOPED_THREAD_H_

#include <thread>
#include <stdexcept>


class scoped_thread
{
    std::thread t;
public:
    template<typename... Args>
    scoped_thread( Args&&... args )
       : t( std::forward<Args>(args)... ){ }
    explicit scoped_thread(std::thread&& t_)
        : t(std::move(t_))
    {
        if(!t.joinable()){
            throw std::logic_error("No thread");
        }
    }
    // scoped_thread(scoped_thread&& rhs)
    //     : t(std::move(rhs.t)) { }
    scoped_thread(scoped_thread&&) = default;

/* .joinable() test is needed if we intend to use the scoped_thread
** in containers. Ommiting the test causes std::sytem_error "Invalid argument"
** to be thrown. Not sure why at the moment */
    ~scoped_thread()
        { if(t.joinable()) t.join(); }

    scoped_thread(const scoped_thread&) = delete;
    scoped_thread& operator=(const scoped_thread) = delete;
    scoped_thread& operator=(scoped_thread&&) = delete;
};


#endif
