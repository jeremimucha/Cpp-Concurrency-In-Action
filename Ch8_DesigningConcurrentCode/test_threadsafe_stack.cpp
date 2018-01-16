#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include "threadsafe_stack.hpp"

using std::string;
using std::cout;
using std::endl;

namespace
{
std::default_random_engine re{
    std::chrono::steady_clock::now().time_since_epoch().count()};
}

int main()
{
    threadsafe_stack<string> tss;
    auto push_action( [&tss]{
        std::uniform_int_distribution<> ud(100, 400);
        for(unsigned i=0; i<11; ++i){
            tss.push("Hi from thread " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
        }
    });
    auto push_action_2( [&tss]{
        std::uniform_int_distribution<> ud(50,150);
        for(;;){
            tss.push("dupablada");
            std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
        }
    });
    auto pop_action( [&tss]{
        for(;;){
            const auto data = *tss.wait_and_pop();
            std::cout << "Got from stack: " << data << endl;
        }
    });

    std::thread push_thread(push_action);
    std::thread(push_action_2).detach();
    std::thread(pop_action).detach();

    push_thread.join();

    std::cout << "\ndone\n" << endl;
}
