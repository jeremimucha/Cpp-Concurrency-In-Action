#include "hierarchical_mutex.hpp"

thread_local unsigned long
hirearchical_mutex::this_thread_hierarchy_value{ULONG_MAX};


int main()
{
    
}