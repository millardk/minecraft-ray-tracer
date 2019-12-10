cmake_minimum_required(VERSION 3.14)
project(path_tracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(path_tracer main.cpp rt.cpp linalg.cpp Json.cpp image.cpp material.cpp material.h)