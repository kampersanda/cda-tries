project(CDA_TRIES)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set(SOURCE_FILES
  lib/BC.cpp
  lib/BitArray.cpp
  lib/Builder.cpp
  lib/CodeTable.cpp
  lib/DacBC.cpp
  lib/DaTrie.cpp
  lib/FastDacBC.cpp
  lib/FlatArray.cpp
  lib/PlainBC.cpp
  lib/main.cpp
  )

add_executable(cda-tries ${SOURCE_FILES})
