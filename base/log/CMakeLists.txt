cmake_minimum_required(VERSION 3.16)

PROJECT(lithe)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")

include_directories( ${PROJECT_SOURCE_DIR})

SET(LIB_SRC
    logStream.cpp
    ../fileUtils.cpp
    ../processInfo.cpp
    logAppender.cpp 
    logFormatter.cpp 
    logEvent.cpp 
    logger.cpp
    )


add_library(lithe ${LIB_SRC})

add_executable(main 
                main.cpp)     
                
add_dependencies(main lithe)
target_link_libraries(main lithe)  

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)