#include <iostream>
#include <string>
#include "threadsafe_queue.hpp"



int main()
{
    threadsafe_queue<std::string> tsq;

    for(int i=0; i<10; ++i){
        tsq.emplace(i+1, 'A');
        tsq.push("Y tho?");
    }

    for(std::string s; tsq.try_pop(s); ){
        std::cout << s << std::endl;
    }
}
