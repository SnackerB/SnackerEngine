cmake_minimum_required(VERSION 3.20)
project(SERP)

set(CMAKE_CXX_STANDARD 20)

add_executable(SERP main.cpp server.h server.cpp SMP.h SERP.h network.h network.cpp SERP.cpp SMP.cpp)
