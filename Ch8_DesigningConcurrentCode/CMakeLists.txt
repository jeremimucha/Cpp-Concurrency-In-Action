cmake_minimum_required(VERSION 3.1)

project(DesigningConcurrentCode)

set( CMAKE_CXX_STANDARD 17 )

add_subdirectory( tests )

file( GLOB target_sources RELATIVE "${PROJECT_SOURCE_DIR}"
      "${PROJECT_SOURCE_DIR}/*.cpp" )

foreach( target ${target_sources} )
  string(REGEX MATCH "^[^ .]*" fname ${target} )
  MESSAGE( STATUS "Executable: ${fname}" )
  add_executable( ${fname} ${target} )
  target_include_directories( ${fname} PUBLIC ${PROJECT_SOURCE_DIR} )
endforeach(target)
