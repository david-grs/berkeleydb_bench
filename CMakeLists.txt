cmake_minimum_required(VERSION 2.8)

project(bdb_benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -g")

add_executable(writer writer.cpp)
target_link_libraries(writer db_stl db_cxx)

add_executable(reader reader.cpp)
target_link_libraries(reader db_stl db_cxx)

add_executable(kyoto_writer kyoto_writer.cpp)
target_link_libraries(kyoto_writer kyotocabinet)

add_executable(kyoto_reader kyoto_reader.cpp)
target_link_libraries(kyoto_reader kyotocabinet)

