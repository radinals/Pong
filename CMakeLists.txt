cmake_minimum_required(VERSION 3.2)

project(
  foo
  VERSION 0.0
  DESCRIPTION "foobar"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(foo src/main.cpp)

target_link_libraries(foo ${SDL2_LIBRARIES})
