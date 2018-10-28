cmake_minimum_required(VERSION 3.12)
project(MyDB)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)