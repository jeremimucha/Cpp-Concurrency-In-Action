/* 
** Special case of protecting data - only when the data is being initialized.
** Using a mutex is possible - but incurs too large graniularity - the data
** is locked always - not only during initialization but also during read-only
** access, which is unnecessary. std::once_flag and std::call_once can be used
** in situations where shared data needs to be locked only during initialization
*/
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <memory>

struct some_resource
{
    // this would normally be some shared resource that requires initialization
    // like a database connection.
    int data{42};
    void do_someting() const
    {
        std::cout << "thread[" << std::this_thread::get_id()
            << "] some resource: data = " << data << std::endl;
    }
};


// initialization using a mutex
// ----------------------------------------------------------------------------
std::shared_ptr<some_resource> mx_resource_ptr;
std::mutex resource_mutex;

void do_stuff_with_resource_mx()
{
    if(!mx_resource_ptr){   // check if data is available to avoid locking unnecessaryli
        // if data unavailable - lock and check if another thread hasn't initialized it
        // in the mean time
        std::lock_guard<std::mutex> lk(resource_mutex);
        if(!mx_resource_ptr){
            mx_resource_ptr.reset(new some_resource);
        }
    }
    mx_resource_ptr->do_someting();
}
// ----------------------------------------------------------------------------


// initialization using once_flag and call_once
// this is faster (and simpler) than using a mutex
// ----------------------------------------------------------------------------
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;

void init_resource()
{
    resource_ptr.reset(new some_resource);
}

void do_stuff_with_resource()
{
    std::call_once(resource_flag, init_resource);   // any callable can be used
    resource_ptr->do_someting();
}
// ----------------------------------------------------------------------------

// using once_flag and call_once for lazy initialization of class members
class lazy_class
{
    std::shared_ptr<std::stringstream> p_ss;
    std::once_flag ss_flag;

    void initialize_ss(){ p_ss.reset(new std::stringstream); }
public:
    constexpr lazy_class() = default;
    void print_data()
    {
        std::call_once(ss_flag, &lazy_class::initialize_ss, this);
        std::cout << "thread[" << std::this_thread::get_id()
            << "] data = " << p_ss->str() << std::endl;
    }

    void update_data(){
        std::call_once(ss_flag, &lazy_class::initialize_ss, this);
        std::cout << "thread[" << std::this_thread::get_id()
            << "] updating data" << std::endl;
        *p_ss << std::this_thread::get_id();
    }

};

// if a single global instance is needed - a static object can be used
// under C++11 memory model static data can be safely initialized without any
// explicit locking mechanisms
// ----------------------------------------------------------------------------
class myclass
{
    std::shared_ptr<int> p_int;
public:
    myclass() : p_int(std::make_shared<int>(42)) { }
    void print_data()
    {
        std::cout << "thread[" << std::this_thread::get_id()
            << "] myclass data = " << *p_int << std::endl;
    }
};
myclass& get_myclass_instance()
{
    static myclass instance;
    return instance;
}
// ----------------------------------------------------------------------------

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
    // using a mutex for data initialization
    auto action = []{do_stuff_with_resource_mx();};
    std::thread mxthread1(periodic_action<decltype(action)>(
            action,500, 1000, 2));
    std::thread mxthread2(periodic_action<decltype(action)>(
            action,500, 1000, 2));

    // using once_flag and call_once to initialize data
    auto call_once_action = []{do_stuff_with_resource();};
    std::thread oncethread1(periodic_action<decltype(call_once_action)>(
        call_once_action, 500, 1000, 2
    ));
    std::thread oncethread2(periodic_action<decltype(call_once_action)>(
        call_once_action, 500, 1000, 2
    ));

    // initialization of class members using call_once
    lazy_class lazy_obj;
    auto update_action = [&lazy_obj]{lazy_obj.update_data();};
    auto print_action = [&lazy_obj]{lazy_obj.print_data();};
    std::thread uthread1(periodic_action<decltype(update_action)>(
        update_action, 500, 600, 5
    ));
    std::thread uthread2(periodic_action<decltype(update_action)>(
        update_action, 500, 600, 5
    ));

    std::thread pthread1(periodic_action<decltype(print_action)>(
        print_action, 500, 600, 5
    ));
    std::thread pthread2(periodic_action<decltype(print_action)>(
        print_action, 500, 600, 5
    ));

    // accessing one global static instance safely initialized without
    // explicit locking
    auto global_action = []{get_myclass_instance().print_data();};
    std::thread gthread1(periodic_action<decltype(global_action)>(
        global_action, 500, 700, 3
    ));
    std::thread gthread2(periodic_action<decltype(global_action)>(
        global_action, 500, 700, 3
    ));

    mxthread1.join(); mxthread2.join();
    oncethread1.join(); oncethread2.join();
    uthread1.join(); uthread2.join();
    pthread1.join(); pthread2.join();
    gthread1.join(); gthread2.join();
}
