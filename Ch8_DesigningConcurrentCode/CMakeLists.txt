cmake_minimum_required(VERSION 3.0)

project(DesigningConcurrentCode)

set( CMAKE_CXX_STANDARD 17 )

add_subdirectory( libs/Catch2 )


###############################################################################
# Unit Tests

# enable CTest testing
enable_testing()

# Add a testing executable
add_executable( catch_unit_tests catch_unit_tests.cpp )

target_link_libraries( catch_unit_tests
                       Catch::Test
                     )

add_test( test_all catch_unit_tests )
