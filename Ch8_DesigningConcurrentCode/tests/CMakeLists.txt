cmake_minimum_required(VERSION 3.1)

project(TestDesigningConcurrentCode)

add_subdirectory( libs/catch )

###############################################################################
# Unit Tests

file( GLOB TestsSources RELATIVE
      "${PROJECT_SOURCE_DIR}"
      "${PROJECT_SOURCE_DIR}/catchtest*.cpp"
    )

# enable CTest testing
enable_testing()

# Add a testing executable
add_executable( catch_unit_tests catch_unit_tests.cpp )
target_include_directories( catch_unit_tests
                            PUBLIC ${CMAKE_SOURCE_DIR}
                          )
target_sources( catch_unit_tests PUBLIC ${TestsSources} )
target_link_libraries( catch_unit_tests
                       Catch::Test
                     )

add_test( test_all catch_unit_tests )
