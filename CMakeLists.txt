cmake_minimum_required(VERSION 3.10)

project(SimpleSort)

set(CMAKE_CXX_STANDARD 11)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} raylib)