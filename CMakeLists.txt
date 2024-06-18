cmake_minimum_required(VERSION 3.2)

project(
  Pong
  VERSION 0.0
  DESCRIPTION "Simple Pong Game"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Pong
  src/main.cpp
  src/Game/game.cpp
  src/Game/game.hpp
  src/Game/letters.hpp
)

target_link_libraries(Pong ${SDL2_LIBRARIES})
