cmake_minimum_required(VERSION 3.15)
project(llcpp VERSION 1.0 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Lib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Tests.cmake)
