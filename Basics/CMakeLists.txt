cmake_minimum_required(VERSION 4.1)
project(Basics)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
enable_testing()

add_library(Basics STATIC library.cpp
        linkedlist.hpp
        queue.hpp
        threadManager.hpp
        hashMap.hpp
        runnable.hpp
        workItem.hpp)
target_include_directories(Basics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(LinkedListTests tests/linkedlist_tests.cpp
        queue.hpp
        threadManager.hpp)
target_link_libraries(LinkedListTests PRIVATE Basics)
add_test(NAME linkedlist_tests COMMAND LinkedListTests)

add_executable(QueueTests tests/queue_tests.cpp
        queue.hpp
        threadManager.hpp)
target_link_libraries(QueueTests PRIVATE Basics)
add_test(NAME queue_tests COMMAND QueueTests)

add_executable(HashMapTests tests/hashMap_tests.cpp
        hashMap.hpp
        linkedlist.hpp)
target_link_libraries(HashMapTests PRIVATE Basics)
add_test(NAME hashMap_tests COMMAND HashMapTests)

add_executable(AllTests
        tests/all_tests.cpp
        tests/linkedlist_tests.cpp
        tests/queue_tests.cpp
        tests/hashMap_tests.cpp)
target_compile_definitions(AllTests PRIVATE BASICS_COMBINED_TEST_RUNNER)
target_link_libraries(AllTests PRIVATE Basics)
add_test(NAME all_tests COMMAND AllTests)
