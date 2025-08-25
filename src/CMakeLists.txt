cmake_minimum_required(VERSION 3.10)

project(physics_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/opt/homebrew/include /opt/homebrew/include/SDL3)

link_directories(/opt/homebrew/lib)

add_executable(func main.cpp)

target_link_libraries(func SDL3)

