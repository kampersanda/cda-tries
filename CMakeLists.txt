cmake_minimum_required(VERSION 2.6)
project(CDA_TRIES)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(ENABLE_YCHECK "Enable YCHECK" ON)
if(ENABLE_YCHECK)
  add_definitions(-DENABLE_YCHECK)
endif()

message(STATUS "BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message(STATUS "ENABLE_YCHECK is ${ENABLE_YCHECK}")

add_subdirectory(lib)
include_directories(lib)

add_executable(Benchmark Benchmark.cpp)
target_link_libraries(Benchmark cda-tries)

enable_testing()
file(GLOB TEST_SOURCES Test*.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
  get_filename_component(TEST_SOURCE_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_SOURCE_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_SOURCE_NAME} cda-tries)
  add_test(
    NAME ${TEST_SOURCE_NAME}
    COMMAND $<TARGET_FILE:${TEST_SOURCE_NAME}>
  )
endforeach(TEST_SOURCE)

add_subdirectory(previous)
