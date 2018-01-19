/* 
** This is a Catch2 unit-test file for the parallel implementations of:
** for_each_parallel_task
** for_each_async
** find_parallel_task
** find_async
** parallel_partial_sum
*/
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <list>
#include <random>
#include <chrono>

namespace
{

std::default_random_engine re{
    std::chrono::steady_clock::now().time_since_epoch().count()};
}

TEST_CASE( "Parallel algorithms work", "[all]" )
{
    std::list<int> test_list;
    std::uniform_int_distribution<> ud;
    for(unsigned i=0; i<1000; ++i){
        test_list.push_back(ud(re));
    }

    REQUIRE( test_list.size() == 1000 );

    SECTION( "for_each_packaged_task works" ){
        auto action = 
    }
}