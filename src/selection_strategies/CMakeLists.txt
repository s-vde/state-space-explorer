cmake_minimum_required(VERSION 3.5)

include_directories(..)
include_directories(../sufficient_sets)

add_library(CustomSelectionStrategies 
  custom_selector_register.cpp
  sleep_sets.cpp
  ../dependence.cpp
  ../sufficient_sets/sleep_set.cpp
)
