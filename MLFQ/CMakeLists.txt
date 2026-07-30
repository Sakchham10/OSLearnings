cmake_minimum_required(VERSION 4.1)
project(MLFQ)

set(CMAKE_CXX_STANDARD 20)

add_executable(MLFQ main.cpp
        MLFQ.hpp
        Task.hpp)

if(NOT TARGET Basics)
    add_subdirectory(../Basics Basics_build)
endif()

target_link_libraries(MLFQ PRIVATE Basics)