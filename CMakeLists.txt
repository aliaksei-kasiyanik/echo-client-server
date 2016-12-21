cmake_minimum_required(VERSION 3.6)
project(echo_client_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES sever.cpp client.cpp)
add_executable(echo_client_server ${SOURCE_FILES})