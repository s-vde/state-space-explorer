cmake_minimum_required(VERSION 3.5.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-lboost_system -lboost_filesystem -lboost_program_options")

####################
# DEPENDENCIES

set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/libs/cpp-utils)
include_directories(${CPP_UTILS}/src)

set(RECORD_REPLAY   ${CMAKE_CURRENT_SOURCE_DIR}/libs/record-replay)
set(PROGRAM_MODEL   ${RECORD_REPLAY}/src/program-model)
include_directories(${PROGRAM_MODEL})

set(PROGRAM_MODEL    ${RECORD_REPLAY}/src/program-model)
add_subdirectory(${PROGRAM_MODEL}   ${CMAKE_CURRENT_BINARY_DIR}/program-model)

set(SCHEDULER   ${RECORD_REPLAY}/src/scheduler)
set(CUSTOM_STRATEGIES   ${CMAKE_CURRENT_SOURCE_DIR}/src/selection_strategies)
include_directories(${SCHEDULER})
add_subdirectory(${SCHEDULER}   ${CMAKE_CURRENT_BINARY_DIR}/scheduler)

set(LLVM_PASS   ${RECORD_REPLAY}/src/llvm-pass)
add_subdirectory(${LLVM_PASS}   ${CMAKE_CURRENT_BINARY_DIR}/llvm-pass)


####################
# INCLUDES

include_directories(src)
include_directories(src/selection_strategies)
include_directories(src/sufficient_sets)


####################
# SOURCES

set(BOUND_FUNCTIONS_SOURCES 
  src/bound_functions/local_bound_function.hpp
  src/bound_functions/preemptions.cpp
)

set(SUFFICIENT_SETS_SOURCES
  src/sufficient_sets/bound_persistent_set.cpp
  src/sufficient_sets/persistent_set.cpp
  src/sufficient_sets/sleep_set.cpp
  src/sufficient_sets/source_set.cpp
  src/sufficient_sets/sufficient_set.cpp
)

set(SCHEDULER_SOURCES
  ${RECORD_REPLAY}/src/scheduler/program.cpp
  ${RECORD_REPLAY}/src/scheduler/schedule.cpp 
  ${RECORD_REPLAY}/src/scheduler/scheduler_settings.cpp 
  ${RECORD_REPLAY}/src/scheduler/replay.cpp
)

set(UTILS_SOURCES
  ${CPP_UTILS}/src/color_output.cpp
  ${CPP_UTILS}/src/utils_io.cpp
)


####################
# EXECUTABLES

add_executable(depth_first_search
  src/bound.cpp
  src/depth_first_search.cpp
  src/exploration.cpp
  src/run_depth_first_search.cpp
  ${BOUND_FUNCTIONS_SOURCES}
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)

target_link_libraries(depth_first_search RecordReplayProgramModel)

add_executable(dpor
  src/bound.cpp
  src/dependence.cpp
  src/depth_first_search.cpp
  src/dpor.cpp
  src/exploration.cpp
  src/happens_before.cpp
  src/run_dpor.cpp
  src/vector_clock.cpp
  ${SCHEDULER_SOURCES}
  ${SUFFICIENT_SETS_SOURCES}
  ${UTILS_SOURCES}
)

target_link_libraries(dpor RecordReplayProgramModel)

add_executable(bounded_search
  src/bound.cpp
  src/depth_first_search.cpp
  src/exploration.cpp
  src/run_bounded_search.cpp
  ${BOUND_FUNCTIONS_SOURCES}
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)

target_link_libraries(bounded_search RecordReplayProgramModel)
