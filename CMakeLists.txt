cmake_minimum_required(VERSION 3.5)

project(nightfear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NIGHTFEARSX_SRC "NULL")

include(cmake/target.cmake)

find_package(SDL2 2.0 REQUIRED)

add_executable(nightfear ${NIGHTFEARSX_SRC})
