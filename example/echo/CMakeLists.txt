if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
    project(ECHO_SERVER VERSION 0.1.0)
else()
    project(ECHO_SERVER)
endif()

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD) # Do not allow hidden visibility for static libs
endif()

find_package(serverpp REQUIRED)
find_package(gsl-lite REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

# The required C++ Standard for Server++ is C++14.
set(CMAKE_CXX_STANDARD 14)

add_executable(echo_server)

target_sources(echo_server
    PRIVATE
        src/echo_server.cpp
)

target_link_libraries(echo_server KazDragon::serverpp)
