cmake_minimum_required(VERSION 3.5.2)

set(CMAKE_CXX_STANDARD 14)

####################
# DEPENDENCIES

find_package(Boost COMPONENTS program_options filesystem system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(GOOGLE_TEST   ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest)
add_subdirectory(${GOOGLE_TEST})
message(STATUS "Using GOOGLE_TEST ${GOOGLE_TEST}")

set(LLVM_DIR    ${LLVM_BUILD_DIR}/lib/cmake/llvm)

set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/libs/cpp-utils)
add_subdirectory(${CPP_UTILS}/tests)
include_directories(${CPP_UTILS}/src)

set(RECORD_REPLAY   ${CMAKE_CURRENT_SOURCE_DIR}/libs/record-replay)

set(PROGRAM_MODEL   ${RECORD_REPLAY}/src/program-model)
include_directories(${PROGRAM_MODEL})

set(SCHEDULER   ${RECORD_REPLAY}/src/scheduler)
set(CUSTOM_STRATEGIES   ${CMAKE_CURRENT_SOURCE_DIR}/src/selection_strategies)
include_directories(${SCHEDULER})

add_subdirectory(${RECORD_REPLAY})

add_subdirectory(tests)


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
  src/sufficient_sets/persistent_set.cpp
  src/sufficient_sets/sleep_set.cpp
  src/sufficient_sets/sufficient_set.cpp
)

set(SCHEDULER_SOURCES
  ${CPP_UTILS}/src/fork.cpp
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

target_compile_definitions(depth_first_search PRIVATE "LLVM_BIN=${LLVM_BIN}")
target_compile_definitions(depth_first_search PRIVATE "RECORD_REPLAY_BUILD_DIR=${RECORD_REPLAY_BUILD_DIR}")

target_link_libraries(depth_first_search RecordReplayProgramModel ${Boost_LIBRARIES})

add_executable(dpor
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

target_compile_definitions(dpor PRIVATE "LLVM_BIN=${LLVM_BIN}")
target_compile_definitions(dpor PRIVATE "RECORD_REPLAY_BUILD_DIR=${RECORD_REPLAY_BUILD_DIR}")

target_link_libraries(dpor RecordReplayProgramModel ${Boost_LIBRARIES})

add_executable(bounded_search
  src/bound.cpp
  src/depth_first_search.cpp
  src/exploration.cpp
  src/run_bounded_search.cpp
  ${BOUND_FUNCTIONS_SOURCES}
  ${SCHEDULER_SOURCES}
  ${UTILS_SOURCES}
)

target_compile_definitions(bounded_search PRIVATE "LLVM_BIN=${LLVM_BIN}")
target_compile_definitions(bounded_search PRIVATE "RECORD_REPLAY_BUILD_DIR=${RECORD_REPLAY_BUILD_DIR}")

target_link_libraries(bounded_search RecordReplayProgramModel ${Boost_LIBRARIES})
