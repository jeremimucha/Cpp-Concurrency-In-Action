#include <iostream>
#include <thread>
#include <string>
#include <list>
#include "quicksort_pending_chunks.hpp"


using std::list;
using std::string;


int main()
{
    list<string> vs{
        "some"
    ,   "goddamn"
    ,   "input"
    ,   "for"
    ,   "the"
    ,   "quicksort"
    ,   "parallel"
    ,   "algorithm"
    ,   "using"
    ,   "threadsafe_stack"
    ,   "implementation"
    };

    std::cout << "list before parallel_quicksort:" << std::endl;
    for(auto it = vs.cbegin(); it != vs.cend(); ++it){
        std::cout << "\t" << *it;
        if( it != vs.cend() ) std::cout << ", ";
        std::cout << std::endl;
    }

    auto sorted_vs = parallel_quicksort(vs);

    std::cout << "\nlist after parallel_quicksort:" << std::endl;
    for(auto it = sorted_vs.cbegin(); it != sorted_vs.cend(); ++it){
        std::cout << "\t" << *it;
        if( it != sorted_vs.cend() ) std::cout << ", ";
        std::cout << std::endl;
    }
}
