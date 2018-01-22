cmake_minimum_required(VERSION 3.0)

project(catch)

# Prepare "Catch2" library for other executables
add_library(Catch INTERFACE)
add_library(Catch::Test ALIAS Catch)
target_include_directories(Catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
