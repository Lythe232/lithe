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
#-fsanitize=address 内存检查
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic  -O0 -ggdb -std=c++11 -fPIC -Wall -Wno-deprecated -Wno-unused-function")

include_directories(${CMAKE_SOURCE_DIR}/)

SET(LIB_SRC
    src/util/string_util.cpp
    src/log/logger.cpp 
    src/log/logAppender.cpp 
    src/log/logFormatter.cpp 
    src/log/logStream.cpp
    src/log/AsyncLogging.cpp
    src/log/logFile.cpp
    src/thread/thread.cpp
    src/thread/currentThread.cpp
    src/thread/condition.cpp
    src/common/fileUtils.cpp 
    src/common/processInfo.cpp
    src/common/timestamp.cpp
    src/common/countDownLatch.cpp
    src/poller.cpp 
    src/epoll_poller.cpp
    src/coroutine.cpp 
    src/coScheduler.cpp
    src/coEpoll.cpp
    src/coEpollItem.cpp
    src/coTcpServer.cpp
    src/coTcpConnection.cpp
    src/channel.cpp
    src/timer.cpp
    src/hookSysCall.cpp
    src/fdmanager.cpp
    src/byteStream.cpp
    src/buffer.cpp
    src/socket.cpp
    src/address.cpp
    src/addressImpl.cpp
    src/eventLoop.cpp
    src/eventLoopThread.cpp
    src/eventLoopThreadPool.cpp
    src/acceptor.cpp
    src/tcpConnection.cpp
    src/tcpServer.cpp
    src/http/httpRequest.cpp
    src/http/httpResponse.cpp
    src/http/httpSession.cpp
    src/http/httpServer.cpp
    )


add_library(lithe ${LIB_SRC})
target_link_libraries(lithe dl)
target_link_libraries(lithe pthread)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

link_libraries(lithe)

add_executable(echoServerCV example/echo/serverCV.cpp)
add_executable(echoServer example/echo/server.cpp)
add_executable(httpServer example/http/main.cpp)


# target_link_libraries(server lithe)

# SET(LIBRARY_OUTPUT_PATH /home/mingyang/Codes/lithe/test)
