#include <future>
#include <cassert>
#include "threadsafe_queue.hpp"


void test_concurrent_push_and_pop_on_empty_queue()
{
    threadsafe_queue<int> q;

    std::promise<void> go, push_ready, pop_ready;
    std::shared_future<void> ready{go.get_future()};

    std::future<void> push_done;
    std::future<int> pop_done;

    try{
        push_done = std::async(std::launch::async,
                        [&q, ready, &push_ready]()
                        {
                            push_ready.set_value();
                            ready.wait();
                            q.push(42);
                        });

        pop_done = std::async(std::launch::async,
                        [&q, ready, &pop_ready]()
                        {
                            pop_ready.set_value();
                            ready.wait();
                            int ret_val{0};
                            q.wait_and_pop(ret_val);
                            return ret_val;
                        });
        
        push_ready.get_future().wait();
        pop_ready.get_future().wait();
        go.set_value();

        push_done.get();
        assert(pop_done.get() == 42);
        assert(q.empty());
    }
    catch(...){
        go.set_value();
        throw;
    }
}


int main()
{
    test_concurrent_push_and_pop_on_empty_queue();
}
