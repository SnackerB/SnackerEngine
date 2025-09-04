cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 23)
project(Network)
add_compile_definitions(_LINUX)

ADD_LIBRARY( Network STATIC
    Network.cpp
    TCP/TCP.cpp
    TCP/TCPEndpoint.cpp
    SERP/SERP.cpp
    SERP/SERPEndpoint.cpp
    SERP/SERPID.cpp
    SERP/StatusCodes.cpp)

target_include_directories(Network PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)