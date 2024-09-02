cmake_minimum_required(VERSION 3.7)
project(chip-8-cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/chip8.cpp src/chip8.hpp src/display.cpp src/display.hpp)
add_executable(chip8 ${SOURCE_FILES})

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8 ${SDL2_LIBRARIES})
