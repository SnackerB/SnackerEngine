cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 23)
project(Math)

ADD_LIBRARY( Math STATIC
    Vec.cpp
    VectorAlgorithms.cpp)

target_include_directories(Math PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)