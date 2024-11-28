cmake_minimum_required(VERSION 3.10)

project(SimpleSort)

set(CMAKE_CXX_STANDARD 11)

find_package(raylib REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include) #

link_directories(${CMAKE_SOURCE_DIR}/libs) #

add_executable(${PROJECT_NAME} main.cpp BattleTime.h)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} enet) #