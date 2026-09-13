cmake_minimum_required(VERSION 3.10)
project(ExtreamFS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

file(GLOB_RECURSE FUENTES "src/*.cpp")

add_executable(extreamfs ${FUENTES})
target_link_libraries(extreamfs Threads::Threads)