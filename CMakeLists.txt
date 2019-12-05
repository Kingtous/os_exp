cmake_minimum_required(VERSION 3.13)
project(os_exp)

set(CMAKE_CXX_STANDARD 11)

add_executable(page cdef.h file_system.cpp file_system.h)