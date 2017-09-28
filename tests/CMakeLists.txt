cmake_minimum_required(VERSION 3.5)

project(state_space_explorer_test)

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/cpp-utils)
include_directories(${CPP_UTILS}/src)

set(RECORD_REPLAY   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/record-replay)
set(SCHEDULER   ${RECORD_REPLAY}/src/scheduler)
include_directories(${SCHEDULER})


####################
# SOURCES

set(SUFFICIENT_SETS_SOURCES
  src/sufficient_sets/persistent_set.cpp
  src/sufficient_sets/sleep_set.cpp
  src/sufficient_sets/sufficient_set.cpp
)

set(SCHEDULER_SOURCES
  ${RECORD_REPLAY}/src/scheduler/schedule.cpp 
  ${RECORD_REPLAY}/src/scheduler/scheduler_settings.cpp 
  ${RECORD_REPLAY}/src/scheduler/replay.cpp
)

set(UTILS_SOURCES
  ${CPP_UTILS}/src/color_output.cpp
  ${CPP_UTILS}/src/utils_io.cpp
)


####################
# LIBRARY

add_executable(StateSpaceExplorerTest
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/dependence.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/depth_first_search.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/dpor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/exploration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/happens_before.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/vector_clock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/sufficient_sets/persistent_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/sufficient_sets/sleep_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/sufficient_sets/sufficient_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dpor_TEST.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main_TEST.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_clock_TEST.cpp
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)


####################
# COMPILE DEFINITIONS

target_compile_definitions(StateSpaceExplorerTest PRIVATE "LLVM_BIN=${LLVM_BUILD_DIR}/bin")
target_compile_definitions(StateSpaceExplorerTest PRIVATE "RECORD_REPLAY_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}/../libs/record-replay")
target_compile_definitions(StateSpaceExplorerTest PRIVATE "TESTS_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions(StateSpaceExplorerTest PRIVATE "TEST_PROGRAMS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_programs")


####################
# LINKING

target_link_libraries(StateSpaceExplorerTest RecordReplayProgramModel)
