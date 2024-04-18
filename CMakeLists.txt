cmake_minimum_required(VERSION 3.22.0)
project(WebServer)

if (WIN32)
    message(FATAL_ERROR "Windows is not supported")
endif()
if (APPLE)
    message(FATAL_ERROR "MacOS is not supported")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
# set(CMAKE_BUILD_TYPE debug)

set(SOURCES
    log/logbuffer.cpp
    log/logger.cpp
    log/logworker.cpp
    poller/poller.cpp
    threadpool/threadpool.cpp
    threadpool/thread.cpp
    http/httphandler.cpp
    webserver/webserver.cpp
    main.cpp
)

set(OUTPUT_NAME "webserver")
add_executable(${OUTPUT_NAME} ${SOURCES})

install(TARGETS ${OUTPUT_NAME} DESTINATION /usr/local/bin)
install(DIRECTORY DESTINATION /var/www/)
install(DIRECTORY DESTINATION /var/log/webserver/)