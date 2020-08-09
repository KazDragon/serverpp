if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
    project(SERVERPP VERSION 0.0.4)
else()
    project(SERVERPP)
endif()

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD) # Do not allow hidden visibility for static libs
endif()

option(SERVERPP_COVERAGE  "Build with code coverage options")
option(SERVERPP_SANITIZE "Build using sanitizers" "")
message("Building Server++ with config: ${CMAKE_BUILD_TYPE}")
message("Building Server++ with code coverage: ${SERVERPP_COVERAGE}")
message("Building Server++ with sanitizers: ${SERVERPP_SANITIZE}")

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
endif()

# The required C++ Standard for Server++ is C++14.
set(CMAKE_CXX_STANDARD 14)

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate a header suitable for declaring which
# symbols should be included.
include(GenerateExportHeader)

# For producing automatically-generated documentation, we use Doxygen.
find_package(Doxygen)

if (SERVERPP_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SERVERPP_SANITIZE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SERVERPP_SANITIZE}")
endif()

if (SERVERPP_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_library(serverpp)

target_sources(serverpp
    PRIVATE
        src/tcp_server.cpp
        src/tcp_socket.cpp
        include/serverpp/tcp_server.hpp
        include/serverpp/tcp_socket.hpp
        include/serverpp/version.hpp
)

target_link_libraries(serverpp
    PUBLIC
        CONAN_PKG::gsl-lite
        CONAN_PKG::boost
)

set_target_properties(serverpp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION ${SERVERPP_VERSION}
        SOVERSION ${SERVERPP_VERSION}
)

target_include_directories(serverpp
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

generate_export_header(serverpp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/serverpp/detail/export.hpp"
)

# Add a rule for generating documentation
if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY)

    add_custom_target(serverpp_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generate API documentation with Doxygen" VERBATIM
    )
endif()

# Add customizations for packaging
set(CPACK_PACKAGE_NAME "Server++")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Server++")
set(CPACK_PACKAGE_VENDOR "Matthew Chaplain")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${SERVERPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SERVERPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SERVERPP_VERSION_PATCH})
include(CPack)
