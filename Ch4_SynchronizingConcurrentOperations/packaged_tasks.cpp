#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <utility>
#include <string>


int main()
{
    std::packaged_task<void()> task0(
        []{ for(int i=0; ; ++i){
            std::cout << "task0 doing stuff on thread["
            << std::this_thread::get_id() << "] count = " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
// we can run a nameless detached thread;
    std::thread(std::move(task0)).detach();


    std::packaged_task<std::string(int,int)> task1(
        [](int ticks, int delay){
            for(int i=0; i<ticks; ++i){
                std::cout << "task1 doing work on thread[" << std::this_thread::get_id()
                    << "] " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
            return "task1 done!";
        });

    std::future<std::string> result1 = task1.get_future();
    std::thread thread1(std::move(task1), 5, 451);
    
    std::packaged_task<std::string(int,int,double)> task2(
        [](int ticks, int delay, double d){
            for(int i=0; i<ticks; ++i){
                std::cout << "task2 doing work on thread[" << std::this_thread::get_id()
                    << "] i*d = " << i*d << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
            return "task2 done!";
        });

    std::future<std::string> result2 = task2.get_future();
    std::thread thread2(std::move(task2), 7, 543, 3.1415);

// --- wait for result explicitly;
    result1.wait(); 
    std::cout << "--- " << result1.get() << std::endl;
// --- get the result without waiting - the call to .get() will block
// untill the thread running task2 is done - may (will) result in
// messed-up output here - the '***' will be printed before task2 is done
// it will continue printing its looped-messaage until done, only after that
// will the result2.get() return value be printed
    std::cout << "*** " << result2.get() << std::endl;

    thread1.join(); thread2.join();
}
