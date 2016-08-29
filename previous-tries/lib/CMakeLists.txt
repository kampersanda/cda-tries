set(SOURCES
  CdaTrieDic.cpp
  DalfTrieDic.cpp
  PrevDaTrieDic.cpp
  )

add_library(previous STATIC ${SOURCES})
target_link_libraries(previous cda-tries)
