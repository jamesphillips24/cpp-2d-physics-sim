cmake_minimum_required(VERSION 3.10)

project (src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_executable(func main.cpp)
