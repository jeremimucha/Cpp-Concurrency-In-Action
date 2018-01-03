#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include "threadsafe_queue.hpp"


threadsafe_queue<int> data_queue;


bool more_data_to_prepare()
{
    static int i{0};
    return i++ < 11;
}
int prepare_data()
{
    static std::default_random_engine re{
        std::chrono::system_clock::now().time_since_epoch().count()};
    static std::uniform_int_distribution<> ud(300, 800);
    static int i{0};
    std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
    return i++;
}

void process(int i)
{
    std::cout << "thread[" << std::this_thread::get_id()
        << "] data = " << i << std::endl;
}

bool is_last_chunk(int data)
{
    return !(data < 10);
}


void data_preparation_thread()
{
    while(more_data_to_prepare()){
        const int data = prepare_data();
        std::cout << "thread[" << std::this_thread::get_id()
            << "] pushing data = " << data << std::endl;
        data_queue.push(data);
    }
}

void data_processing_thread()
{
    while(true){
        int data;
        data_queue.wait_and_pop(data);
        process(data);
        if(is_last_chunk(data))
            break;
    }
}

int main()
{
    std::thread prep_thread(data_preparation_thread);
    prep_thread.detach();

    std::thread processing_thread(data_processing_thread);
    processing_thread.join();
}
