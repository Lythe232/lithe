cmake_minimum_required(VERSION 3.16)

PROJECT(TEST)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")


include_directories(${CMAKE_SOURCE_DIR}/../)



add_executable(main
                main.cpp
                thread.cpp
                ../log/logger.cpp 
                ../log/logFormatter.cpp 
                ../log/logAppender.cpp 
                ../common/fileUtils.cpp 
                ../common/timestamp.cpp
                ../common/processInfo.cpp
                ../log/logStream.cpp
                )
target_link_libraries(main pthread)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)