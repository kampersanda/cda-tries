set(SOURCES
  Bc.cpp
  BitArray.cpp
  Builder.cpp
  CodeTable.cpp
  DacBc.cpp
  DaTrieDic.cpp
  FastDacBc.cpp
  PlainBc.cpp
  SmallArray.cpp
  Basic.hpp)

add_library(cda-tries STATIC ${SOURCES})
