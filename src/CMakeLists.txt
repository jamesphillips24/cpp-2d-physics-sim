cmake_minimum_required(VERSION 3.10)

project(physics_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include SDL2 headers
include_directories(/opt/homebrew/include /opt/homebrew/include/SDL2)

# Linker search path
link_directories(/opt/homebrew/lib)

# Your executable
add_executable(func main.cpp)

# Link against SDL2 and SDL2_image
target_link_libraries(func SDL2 SDL2_image)

