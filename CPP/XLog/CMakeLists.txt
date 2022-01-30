cmake_minimum_required(VERSION 3.10.2)
project(XLog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(XLog
        src/XLog.cpp
        src/XLogOStringStream.cpp
        src/XLogStringBuf.cpp
)

include(TestBigEndian)

TEST_BIG_ENDIAN(XLOG_COMPILE_ENDIAN_IS_BIG_ENDIAN)

target_compile_definitions(XLog PUBLIC XLOG_COMPILE_ENDIAN_IS_BIG_ENDIAN=${XLOG_COMPILE_ENDIAN_IS_BIG_ENDIAN})

target_include_directories(XLog PUBLIC include)

if (PLATFORM_ANDROID)
    target_link_libraries(
            XLog
            log
    )
endif()