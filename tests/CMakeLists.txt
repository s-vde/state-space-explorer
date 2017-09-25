cmake_minimum_required(VERSION 3.5)

project(state_space_explorer_test)

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

include_directories(${CPP_UTILS}/src)


####################
# LIBRARY

add_executable(RecordReplayTest
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/vector_clock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_clock_TEST.cpp
)
