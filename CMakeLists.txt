cmake_minimum_required(VERSION 3.10.2)
project(cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(CPP)

add_executable(main main.cpp)

target_link_libraries(main CPP)