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

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

# The required C++ Standard for Server++ is C++14.
set(CMAKE_CXX_STANDARD 14)

set (ECHO_SERVER_PUBLIC_SOURCE_FILES
    src/echo_server.cpp
)

set (ECHO_SERVER_PUBLIC_HEADER_FILES
)

add_executable(echo_server
    ${ECHO_SERVER_PUBLIC_HEADER_FILES}
    ${ECHO_SERVER_PUBLIC_SOURCE_FILES}
)

target_link_libraries(echo_server
    CONAN_PKG::serverpp
)
