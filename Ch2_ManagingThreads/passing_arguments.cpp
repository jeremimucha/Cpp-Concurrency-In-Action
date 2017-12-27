#include <iostream>
#include <thread>
#include <functional>

struct some_stuff
{
    int data;
};

void update_data(some_stuff& ss, int i);

class task
{
    some_stuff& ss;
public:
    explicit task( some_stuff& ss_) : ss(ss_) { }
    void operator()(int i)
        { update_data(ss, i); }
};

int main()
{
    some_stuff ss1{1};
    some_stuff ss2{2};
    some_stuff ss3{3};
    some_stuff ss4{4};
    // std::thread t1(update_data, ss1, 42); // compile-time error
    std::thread t2(update_data, std::ref(ss2), 42);
    std::thread t3([&ss3](){ss3.data=42;});
    std::thread t4(task(ss4), 42);
    // std::cout << "ss1: on thread" << t1.get_id() << std::endl;
    std::cout << "ss2: on thread" << t2.get_id() << std::endl;
    std::cout << "ss3: on thread" << t3.get_id() << std::endl;
    std::cout << "ss4: on thread" << t4.get_id() << std::endl;
    /* t1.join(); */ t2.join(); t3.join(); t4.join();
    // std::cout << "ss1: after update: " << ss1.data << std::endl;
    std::cout << "ss2: after update: " << ss2.data << std::endl;
    std::cout << "ss3: after update: " << ss3.data << std::endl;
    std::cout << "ss4: after update: " << ss4.data << std::endl;

}

void update_data(some_stuff& ss,  int i)
{
    ss.data = i;
}