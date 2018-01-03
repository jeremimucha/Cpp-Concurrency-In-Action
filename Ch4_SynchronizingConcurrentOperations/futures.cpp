#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <random>


std::default_random_engine re{std::chrono::system_clock::now().time_since_epoch().count()};
std::uniform_int_distribution<> ud(300,600);


int do_async_stuff();
void do_stuff();


int main()
{
    std::future<int> async_result = std::async(do_async_stuff);
    do_stuff();
    std::cout << "The result of async operation is " << async_result.get()
        << std::endl;
}


int do_async_stuff()
{
    for(int i=0; i<10; ++i){
        std::cout << "async_stuff thread[" << std::this_thread::get_id()
            << "] pretending to be usefull stuff" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
    }
    return 42;
}

void do_stuff()
{
    for(int i=0; i<5; ++i){
        std::cout << "in main thread[" << std::this_thread::get_id()
            << "] doing some stuff while the async operation is running"
            << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
    }
}
