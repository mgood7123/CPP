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

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest)
# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("googletest/include")
endif()

add_executable(CPP_TESTS
    tests/general.cpp
)

target_link_libraries(CPP_TESTS CPP gtest_main)
