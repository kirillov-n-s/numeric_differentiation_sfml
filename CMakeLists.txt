cmake_minimum_required(VERSION 3.20)
project(laba2)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

add_executable(laba2 main.cpp seq.h plot.h)

target_link_libraries(laba2 sfml-graphics sfml-window sfml-system)
