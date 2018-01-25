cmake_minimum_required(VERSION 3.1)

project(ATM)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_BUILD_TYPE Release )

# add_subdirectory( tests )
set( main_source "main.cpp" )

add_executable( atm ${main_source} )
target_include_directories( atm PUBLIC
  ${PROJECT_SOURCE_DIR}
  "${PROJECT_SOURCE_DIR}/MessagePassingFramework" )
