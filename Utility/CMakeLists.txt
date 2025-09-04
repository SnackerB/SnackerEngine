cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 23)
project(Utility)

ADD_LIBRARY( Utility STATIC
    Alignment.cpp
    Animatable.cpp
    AnimationFunctions.cpp
    base64.cpp
    Buffer.cpp
    Conversions.cpp
    Formatting.cpp
    Json.cpp
    Random.cpp
    Timer.cpp
    Handles/EventHandle.cpp
    Handles/VectorEventHandle.cpp)

target_include_directories(Utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)