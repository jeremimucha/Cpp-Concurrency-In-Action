#include "catch.hpp"
#include <list>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include "find_packaged_task.hpp"


namespace
{
std::default_random_engine re{
    std::chrono::steady_clock::now().time_since_epoch().count()};
}


TEST_CASE( "parallel_find_packaged_task", "[find][packaged_task]" )
{
    std::list<int> test_list;
    std::uniform_int_distribution<> ud;
    for(unsigned i=0; i<1000; ++i){
        test_list.push_front( ud(re) );
    }
    test_list.push_back(42);
    for( auto it = std::find(test_list.begin(), test_list.end(), 11);
        it != test_list.end();
        it = std::find(test_list.begin(), test_list.end(), 11) ){
        test_list.erase(it);
    }

    SECTION( "Finds the value" ){
        REQUIRE(parallel_find(test_list.cbegin(), test_list.cend(), 42) ==
            std::find(test_list.cbegin(), test_list.cend(), 42));
    }

    SECTION( "No false-positives" ){
        REQUIRE( parallel_find(test_list.cbegin(), test_list.cend(), 11) ==
            test_list.cend() );
    }
}
