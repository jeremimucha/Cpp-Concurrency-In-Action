#include <iostream>
#include <future>
#include <thread>
#include <string>
#include <chrono>


int main()
{
    std::promise<int> p1;
    std::future<int> f1(p1.get_future());
    std::thread t1([&p1]{
        for(int i=0; i<10; ++i){
            std::cout << "thread[" << std::this_thread::get_id()
                << "] running..." << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(356));
        }
        p1.set_value(42);
    });

    std::promise<std::string> p2;
    std::future<std::string> f2(p2.get_future());
    std::thread t2([&p2]{
        for(int i=0; i<10; ++i){
            std::cout << "thread[" << std::this_thread::get_id()
                << "] running... " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(234));
        }
        p2.set_value("Promisses, promisses..");
    });

    t1.detach(); t2.detach();

    // the threads are detached but we can still retrieve the shared state
    f1.wait(); f2.wait();
    std::cout << "Result of promise1 = " << f1.get() << "\n"
        << "Result of promise2 = " << f2.get() << std::endl;
}
