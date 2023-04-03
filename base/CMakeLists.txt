cmake_minimum_required(VERSION 3.16)

PROJECT(lithe)

SET(CMAKE_VERBOSE_MAKEFILE ON)

# SET(CXX_FLAGS 
#     -rdynamic 
#     -O0
#     -ggdb
#     -std=c++11
#     -Wall 
#     -Wno-deprecated 
#     -Wno-unused-function 
#     )

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")

include_directories(${CMAKE_SOURCE_DIR})

SET(LIB_SRC
    log/logger.cpp 
    log/logAppender.cpp 
    log/logFormatter.cpp 
    log/logStream.cpp
    log/AsyncLogging.cpp
    log/logFile.cpp
    thread/thread.cpp
    thread/condition.cpp
    common/fileUtils.cpp 
    common/processInfo.cpp
    common/timestamp.cpp
    common/countDownLatch.cpp
    )

add_library(lithe ${LIB_SRC})

add_executable(test 
                log/test.cpp
)

add_dependencies(test lithe)

target_link_libraries(test lithe)
target_link_libraries(test pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)