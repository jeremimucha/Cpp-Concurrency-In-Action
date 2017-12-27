/* 
** Basic mutex use for shared data protection in mulithreaded code
** - std::mutex - 'mutual exclusivity' class, provides lock(), try_lock()
**   and unlock() interface
** - std::lock_guard - RAII idiom template which handles locking and unlocking
**   a mutex via construction / destruction.
** - std::lock - a RAII class used to lock multiple mutexes simultaneously
**   in order to avoid possibility of a deadlock. It's use is combined with
**   std::lock_guard (which handles the unlocking of mutexes) and the
**   std::adapt_lock, which is passed to std::lock_guard along with the mutex
**   this avoids the locking of a mutex by std::lock_guard while allowing it
**   to manage it's scope/lifetime
**
** See also list and stack implementations allowing thread-safe
** container operations
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>
#include <algorithm>
#include <random>


std::list<int> some_list;
std::mutex list_mutex;

void add_to_list(int new_value)
{
    std::lock_guard<std::mutex> guard(list_mutex);
    some_list.push_back(new_value);
}

bool list_contains( int value_to_find )
{
    std::lock_guard<std::mutex> guard(list_mutex);
    return std::find(some_list.cbegin(), some_list.cend(), value_to_find)
            != some_list.cend();
}

int list_pop_front()
{
    std::lock_guard<std::mutex> guard(list_mutex);
    if(some_list.empty())
        throw std::runtime_error("empty list");
    // copy the value and remove it from the list while under the mutex
    // in order to avoid race conditions. If list held classes the copy / move
    // operations would need to be noexcept
    int retval = some_list.front();
    some_list.pop_front();
    return retval;
}

template<typename Func>
class periodic_action
{
    static std::default_random_engine re;
    std::uniform_int_distribution<> ud;
    Func func;
public:
    explicit periodic_action(Func f,int min, int max)
        : func(f), ud(min, max) { }
    void operator()()
    {
            func(re, ud);
    }
};

template<typename Func>
std::default_random_engine periodic_action<Func>::re{std::chrono::system_clock::now().time_since_epoch().count()};


int main()
{
    // start a producer thread
    auto fill_list = [](std::default_random_engine& re, std::uniform_int_distribution<>& ue)
                     {  for(int i=0; i<20; ++i){
                         int t = ue(re);
                            add_to_list(t);
                            std::cout << "thread[" << std::this_thread::get_id()
                                << "]: adding " << t << std::endl;
                            std::this_thread::sleep_for(std::chrono::milliseconds(t*10));
                        }
                     };
    std::thread t_producer(periodic_action<decltype(fill_list)>(fill_list,0, 20));

    // start a lookup-thread
    auto lookup_value = [](std::default_random_engine& re, std::uniform_int_distribution<>& ue)
                        {   while(true){
                            int t = ue(re);
                                std::cout << "thread[" << std::this_thread::get_id()
                                << "]: " << t << " found: " << t << " " << std::boolalpha
                                << list_contains(t);
                            std::this_thread::sleep_for(std::chrono::milliseconds(t*50));
                            }
                        };
    std::thread t_checker(periodic_action<decltype(lookup_value)>(lookup_value, 0, 20));
    t_checker.detach();

    // start two consumer threads
    auto get_value = [](std::default_random_engine& re, std::uniform_int_distribution<>& ue)
                     {
                         while(true) try{
                             int t = ue(re);
                             std::cout << "thread[" << std::this_thread::get_id()
                                << "]: got value " << list_pop_front() << std::endl;
                            std::this_thread::sleep_for(std::chrono::milliseconds(t));
                         }
                         catch(std::exception& e){
                             std::cout << "thread[" << std::this_thread::get_id()
                                << "]: list empty." << std::endl;
                                break;
                         }
                     };
    // give the producer a chance to fill the list.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t_consumer1(periodic_action<decltype(get_value)>(get_value, 500, 2000));
    std::thread t_consumer2(periodic_action<decltype(get_value)>(get_value, 500, 2000));
    t_producer.join();
    t_consumer1.join();
    t_consumer2.join();
}
