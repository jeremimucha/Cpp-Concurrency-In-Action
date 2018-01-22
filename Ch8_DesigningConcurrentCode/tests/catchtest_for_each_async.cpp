#include "catch.hpp"
#include <list>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include "for_each_async.hpp"


namespace
{
std::default_random_engine re{
    std::chrono::steady_clock::now().time_since_epoch().count()};

} // namespace

// matchers
/* ------------------------------------------------------------------------- */
// class MyListMatcher : public Catch::MatcherBase<std::list<int>>
// {
//     std::list<int> m_lst;
// public:
//     explicit MyListMatcher( const std::list<int>& lst,  void(*func)(int&) )
//         : m_list( lst )
//         {
//             std::for_each( m_lst.begin(), m_lst.end(), func );
//         }
    
//     virtual bool match( const std::list<int>& lst ) const override
//     {
//         return m_lst == lst;
//     }

//     virtual std::string describe() const override
//     {
//         return "Lists aren't equal";
//     }
// };

// inline MyListMatcher MyListsMatch( const std::list<int>& lst, void(*func)(int&) )
// {
//     return MyListMatcher(lst, func);
// }
/* ------------------------------------------------------------------------- */

TEST_CASE( "parallel_for_each_async", "[for_each][async]" )
{
    std::list<int> test_list;
    std::uniform_int_distribution<> ud;
    for(unsigned i=0; i<1000; ++i){
        test_list.push_back(ud(re));
    }
    auto action = [](int& i){ i+=100; };
    std::list<int> expected( test_list );
    REQUIRE( expected == test_list );
    std::for_each( expected.begin(), expected.end(), action );
    REQUIRE( test_list.size() == 1000 );
    REQUIRE( expected.size() == 1000 );


    SECTION( "Works for large number of elements" ){
        parallel_for_each(test_list.begin(), test_list.end(), action );
        REQUIRE( test_list == expected );
    }

    SECTION( "Works for small number of elements" ){
        std::list<int> test_list2;
        std::uniform_int_distribution<> ud;
        for(unsigned i=0; i<10; ++i){
            test_list2.push_back(i);
        }
        auto expected2( test_list2 );
        REQUIRE( expected2 == test_list2 );
        std::for_each( expected2.begin(), expected2.end(), action );

        parallel_for_each(test_list2.begin(), test_list2.end(), action );
        REQUIRE( test_list2 == expected2 );
    }
}
