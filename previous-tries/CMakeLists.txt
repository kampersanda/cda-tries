add_subdirectory(lib)
include_directories(lib)

add_executable(BenchmarkPrev BenchmarkPrev.cpp)
target_link_libraries(BenchmarkPrev previous)

enable_testing()
file(GLOB TEST_SOURCES Test*.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
  get_filename_component(TEST_SOURCE_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_SOURCE_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_SOURCE_NAME} previous)
  add_test(
    NAME ${TEST_SOURCE_NAME}
    COMMAND $<TARGET_FILE:${TEST_SOURCE_NAME}>
  )
endforeach(TEST_SOURCE)
