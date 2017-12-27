#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include "thread_guard.hpp"
#include "scoped_thread.hpp"



class tick_print
{
    std::default_random_engine re{std::chrono::system_clock::now().time_since_epoch().count()};
    std::uniform_int_distribution<> ud;
public:
    explicit tick_print(int mi, int ma) : ud(mi,ma) { }
    void operator()(std::string msg){
        for(int i=0; i<10 ; ++i){
            int t = ud(re);
            std::cout << msg <<"[" << std::this_thread::get_id() << "]: "
                      << i <<", sleep = " << t << "ms" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
        }
    }
};


int main()
{
    std::vector<std::thread> vt;
    std::vector<scoped_thread> vst;
    {
    std::thread t1(tick_print(500,2000), "thread_guard: ");
    thread_guard tg1(t1);
    const auto core_count = std::thread::hardware_concurrency();
    for(unsigned i=0; i<core_count; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "main: " << std::this_thread::get_id()
                  << ", i: " << i << std::endl;
        // vst.emplace_back(std::thread(tick_print(100*i, 500*i)));
        vst.emplace_back(tick_print(100*i+1, 500*i+1), "scoped_thread");
        vt.emplace_back(tick_print(100*i+1, 500*i+1), "raw thread");
    }
    // all scoped_thread and thread_guard objects 
    // are joined automatically on scope exit
    }
    std::cout << "main thread: " << std::this_thread::get_id() << std::endl;
    for( auto it = vt.begin(); it!= vt.end(); ++it ){
        it->join();
    }
}
