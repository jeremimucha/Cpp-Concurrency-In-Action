/* 
** Race conditions occur when multiple threads acces shared data and at least
** one of the threads is modifying (writing to) the data.
** Race conditions can be avoided by:
** - using locking mechanisms (mutexes)
** - lock-free programming - modifying the data structures and its invariants
**   so that each modification is divided into indivisible changes, while
**   each change preserves the invariant.
** - Software transactional memory - each transaction is a single step, and
**   is commited only if the transaction wasn't interrupted by a different
**   thread.
**
** Using mutexes to avoid race conditions:
** - use RAII - std::lock_guard, std::lock (for multiple mutexes)
** - group data and associated mutex together in a class to encapsulate its use.
** - avoid returning references or pointers to the protected data
** - avoid passing protected data to user functions which might also save
**   references or pointers to it.
** - Keep in mind inherent race conditions of some data structures. E.g.
**   stack's state might change between the call to empty(), top() and pop(),
**   it might be necessary to provide a modified interface.
**   Possible modifications:
**   - return via reference - void pop(T& retval)
**   - require no-throw copy/move constructable 
**   - return a pointer to the poped item - avoids the need for no-throw copy/move
** */

// TODO: add a data-race example (avoiding data races)
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <utility>
#include <vector>


template<typename Action>
class periodic_action
{
    static std::default_random_engine re;
    std::uniform_int_distribution<> ud;
    Action action;
    int ticks;
public:
    explicit periodic_action(Action action_, int low, int high, int ticks_)
        : ud(low, high), action(action_), ticks(ticks_) { }
    template<typename... Args>
    void operator()(Args&&... args){
        if(ticks){
        for(int i=0; i<ticks; ++i){
            int t = ud(re);
            action(std::forward<Args>(args)...);
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
        }
        }
        else{
            while(true){
                int t = ud(re);
                action(std::forward<Args>(args)...);
                std::this_thread::sleep_for(std::chrono::milliseconds(t));
            }
        }
    }
};


template<typename Action>
std::default_random_engine periodic_action<Action>::re{std::chrono::system_clock::now().time_since_epoch().count()};


int main()
{

    unsigned int var{0};
    // start a few threads which add +1 to var and print the value
    auto action = [&var]{
        ++var;
        std::cout <<"thread[" << std::this_thread::get_id()
            << "]: " << var << std::endl;
    };
    // values printed might not be consecutive due to data races
    std::vector<std::thread> vt;
    for(int i=0; i<10; ++i){
        vt.emplace_back(periodic_action<decltype(action)>(action,1000, 1000, 5));
    }
    for(auto it = vt.begin(); it != vt.end(); ++it){
        it->join();
    }

}
