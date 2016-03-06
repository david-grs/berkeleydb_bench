cmake_minimum_required(VERSION 2.8)

project(bdb_benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -g")

add_executable(writer writer.cpp)
target_link_libraries(writer db_stl db_cxx)

