cmake_minimum_required(VERSION 3.28)
project(lob CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(LOB_SANITIZE "Build with address and UB sanitizers" OFF)

add_executable(lob
  src/main.cpp
  src/book.cpp
  src/latency.cpp
)

target_include_directories(lob PRIVATE include)
target_compile_options(lob PRIVATE -Wall -Wextra -Wpedantic)

if(LOB_SANITIZE)
  target_compile_options(lob PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g)
  target_link_options(lob PRIVATE -fsanitize=address,undefined)
endif()
