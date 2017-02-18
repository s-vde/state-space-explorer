cmake_minimum_required(VERSION 3.5.2)

set(CMAKE_CXX_STANDARD 14)

set(CUSTOM_STRATEGIES ${CMAKE_CURRENT_SOURCE_DIR}/src/selection_strategies)
add_subdirectory(${RECORD_REPLAY}/src/scheduler ${CMAKE_CURRENT_BINARY_DIR}/scheduler)

include_directories(src)
include_directories(src/selection_strategies)
include_directories(src/sufficient_sets)
include_directories(${RECORD_REPLAY}/src/scheduler)
include_directories(${PROGRAM_MODEL_SRC})
include_directories(${UTILS_SRC})

set(CMAKE_EXE_LINKER_FLAGS "-lboost_system -lboost_filesystem")

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
  ${PROGRAM_MODEL_SRC}/execution.cpp
  ${PROGRAM_MODEL_SRC}/execution_io.cpp
  ${PROGRAM_MODEL_SRC}/instruction.cpp
  ${PROGRAM_MODEL_SRC}/instruction_io.cpp
  ${PROGRAM_MODEL_SRC}/object.cpp
  ${PROGRAM_MODEL_SRC}/object_io.cpp
  ${PROGRAM_MODEL_SRC}/state.cpp
  ${PROGRAM_MODEL_SRC}/state_io.cpp
  ${PROGRAM_MODEL_SRC}/thread.cpp
  ${PROGRAM_MODEL_SRC}/thread_io.cpp
  ${PROGRAM_MODEL_SRC}/transition.cpp
  ${PROGRAM_MODEL_SRC}/transition_io.cpp
)

set(UTILS_SOURCES
  ${UTILS_SRC}/color_output.cpp
  ${UTILS_SRC}/utils_io.cpp
)

add_executable(depth_first_search
  src/bound.cpp
  src/depth_first_search.cpp
  src/exploration.cpp
  src/run_depth_first_search.cpp
  ${BOUND_FUNCTIONS_SOURCES}
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)

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

add_executable(bounded_search
  src/bound.cpp
  src/depth_first_search.cpp
  src/exploration.cpp
  src/run_bounded_search.cpp
  ${BOUND_FUNCTIONS_SOURCES}
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)
