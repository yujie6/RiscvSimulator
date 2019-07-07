cmake_minimum_required(VERSION 3.5.2)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_executable(code main.cpp namespace.hpp memory.hpp program.hpp)
