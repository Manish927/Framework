cmake_minimum_required(VERSION 3.31)
project(Framework)

set(CMAKE_CXX_STANDARD 20)

add_executable(Framework main.cpp
        src/connection_pool.cpp
        src/connection_pool.h)
