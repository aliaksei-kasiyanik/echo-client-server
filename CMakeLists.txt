cmake_minimum_required(VERSION 3.6)
project(echo_client_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES1 client.cpp)
add_executable(client ${SOURCE_FILES1})

set(SOURCE_FILES2 server.cpp)
add_executable(server ${SOURCE_FILES2})

set(SOURCE_FILES3 manager.cpp)
add_executable(manager ${SOURCE_FILES3})