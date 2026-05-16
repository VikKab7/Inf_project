cmake_minimum_required(VERSION 3.10)
project(snake)

set(CMAKE_CXX_STANDARD 17)

add_executable(snake WIN32
    src/main.cpp
)

target_link_libraries(snake gdi32)