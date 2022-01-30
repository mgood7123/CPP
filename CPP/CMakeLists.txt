cmake_minimum_required(VERSION 3.10.2)
project(CPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(XLog)

add_library(CPP
        src/CPP.cpp
        src/Rules.cpp
)

target_include_directories(CPP PUBLIC include)

target_link_libraries(CPP XLog)
