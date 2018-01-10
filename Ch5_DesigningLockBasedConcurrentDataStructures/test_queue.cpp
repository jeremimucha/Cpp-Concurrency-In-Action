#include <iostream>
#include <memory>
#include <string>
#include "simple_queue.hpp"


int main()
{
    Queue<std::string> q;
    q.push("ONE");
    q.push("TWO");
    q.push("THREE");
    q.emplace(4, '5');
    q.emplace("nope");

    std::shared_ptr<std::string> sptr;
    while( (sptr = q.try_pop()) != nullptr ){
        std::cout << *sptr << std::endl;
    }

}
