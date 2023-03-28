cmake_minimum_required(VERSION 3.16)

PROJECT(lithe)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")

include_directories(${CMAKE_SOURCE_DIR})

SET(LIB_SRC
    log/logger.cpp 
    log/logAppender.cpp 
    log/logFormatter.cpp 
    log/logStream.cpp
    thread/thread.cpp
    common/fileUtils.cpp 
    common/processInfo.cpp
    common/timestamp.cpp
    )

add_library(lithe ${LIB_SRC})

add_executable(main 
                log/main.cpp
)

add_dependencies(main lithe)

target_link_libraries(main lithe)
target_link_libraries(main pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)