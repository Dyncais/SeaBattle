cmake_minimum_required(VERSION 3.10)

project(SimpleSort)

set(CMAKE_CXX_STANDARD 11)

find_package(raylib REQUIRED)
if (DEFINED ENV{ENET_LIBRARY} AND DEFINED ENV{ENET_INCLUDE_DIR})
    include_directories($ENV{ENET_INCLUDE_DIR}) #

    link_directories($ENV{ENET_LIBRARY}) #
else()
    include_directories(${CMAKE_SOURCE_DIR}/include) #

    link_directories(${CMAKE_SOURCE_DIR}/libs) #
endif()

add_executable(${PROJECT_NAME} main.cpp BattleTime.h)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} enet) #
