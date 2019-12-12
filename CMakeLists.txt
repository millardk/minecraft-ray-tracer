cmake_minimum_required(VERSION 3.14)
project(path_tracer)

#set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
set(CMAKE_CXX_STANDARD 17)

add_executable(path_tracer main.cpp rt.cpp linalg.cpp Json.cpp image.cpp)