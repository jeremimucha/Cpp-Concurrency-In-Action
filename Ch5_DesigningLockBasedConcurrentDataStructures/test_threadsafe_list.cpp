#include <iostream>
#include <string>
#include "threadsafe_list.hpp"



int main()
{
    threadsafe_list<std::string> lst;
    lst.push_front("ONE");
    lst.push_front("TWO");
    lst.push_front("THREE");
    lst.push_front("FOUR");
    lst.push_front("FIVE");

    lst.for_each( [](const std::string& s)
                  { std::cout << "for_each: " << s << std::endl; });
    auto sptr = lst.find_first_if([](const std::string& s)
                                  {return s == "THREE";});
    std::cout << "find_first_if returned "
              << ((sptr != nullptr) ? *sptr : "nullptr") << std::endl;
    
    lst.remove_if([](const std::string& s){ return s == "THREE"; });

    std::cout << "THREE got removed" << std::endl;
}
