cmake_minimum_required(VERSION 2.8)
project(CDA_TRIES)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set(SOURCE_FILES
  src/main.cpp
  src/BC.cpp
  src/BitArray.cpp
  src/Builder.cpp
  src/CodeTable.cpp
  src/DacBC.cpp
  src/DaTrie.cpp
  src/FastDacBC.cpp
  src/FlatArray.cpp
  src/PlainBC.cpp
  )

add_executable(cda-tries ${SOURCE_FILES})
