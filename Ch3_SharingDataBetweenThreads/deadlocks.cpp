/* 
** Deadlocks occur when multiple locks are aquired by multiple threads
** to performa an operation. Each thread holds one mutex and is waiting for
** another to be released, which never occurs. 
** 
** Avoiding deadlocks:
** - always acquire mutexes in the same order - easy with the help of
**   std::lock (example below)
** - Deadlocks can be created by calls to join() - two threads are waiting
**   for each other to finish. Avoid by not waiting for a thread if there's
**   a slightest chance that it might be waiting for you (this thread).
** - Avoid nested locks alltogether if possible
** - Avoid calling user-supplied code while holding a lock - it might lock
**   other data.
** - Acquire locks in a fixed order - can be conceptual / by design, or can
**   be enforced by a hirearchical mutex.
*/

// TODO: add deadlock examples (avoiding deadlocks)
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <utility>


std::mutex global_mutex1;
std::mutex global_mutex2;

int main()
{
    auto task1 = [&]{
        std::cout << "Task1 - thread[" << std::this_thread::get_id()
         << "] -- locking global_mutex1." << std::endl;
        std::lock_guard<std::mutex> lk1(global_mutex1);
        std::cout << "Task1 - thread[" << std::this_thread::get_id()
            << "] -- trying to get global_mutex2.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lk2(global_mutex2);
        std::cout << "Task1 - thread[" << std::this_thread::get_id()
            << "] -- Got both locks!" << std::endl;
    };
    auto task2 = [&]{
    std::cout << "Task2 - thread[" << std::this_thread::get_id()
        << "] -- locking global_mutex2." << std::endl;
    std::lock_guard<std::mutex> lk1(global_mutex2);
    std::cout << "Task2 - thread[" << std::this_thread::get_id()
        << "] -- trying to get global_mutex1.";
    std::lock_guard<std::mutex> lk2(global_mutex1);
    std::cout << "Task2 - thread[" << std::this_thread::get_id()
        << "] -- Got both locks!" << std::endl;
    };

    // task1 will lock global_mutex1 first, task2 will lock global_mutex2 first
    // each will try to get the other lock - and will wait forever.

    std::thread thread1(task1);
    std::thread thread2(task2);
    thread1.join(); thread2.join();
    std::cout << "Both tasks done!" << std::endl;
}
