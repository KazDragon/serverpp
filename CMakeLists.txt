cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
cmake_policy(VERSION 3.13)

option(SERVERPP_COVERAGE  "Build with code coverage options")
option(SERVERPP_SANITIZE "Build using sanitizers" "")
message("Building Server++ with config: ${CMAKE_BUILD_TYPE}")
message("Building Server++ with code coverage: ${SERVERPP_COVERAGE}")
message("Building Server++ with sanitizers: ${SERVERPP_SANITIZE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

if (NOT SERVERPP_VERSION)
    include(function-git_version)
    git_version(SERVERPP)
endif()

message(STATUS "Server++ Version: ${SERVERPP_VERSION}")
project(SERVERPP VERSION ${SERVERPP_VERSION})

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message("Using Conan due to conanbuildinfo.cmake")
    set(SERVERPP_USE_CONAN ON)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

if (SERVERPP_USE_CONAN)
    set(SERVERPP_PUBLIC_LIBRARIES
        CONAN_PKG::boost
        CONAN_PKG::gsl-lite
    )

    set(SERVERPP_PRIVATE_LIBRARIES
    )
else()
    # Server++ requires at least Boost 1.69.
    find_package(Boost 1.69.0 REQUIRED COMPONENTS container)

    # Server++ requires at least version 0.38 of gsl-lite
    find_package(gsl-lite 0.38.0 REQUIRED)

    # Server++ requires the system threading library.
    find_package(Threads REQUIRED)

    set(SERVERPP_PUBLIC_LIBRARIES 
        Boost::boost
        gsl::gsl-lite
        Threads::Threads
    )

    set(SERVERPP_PRIVATE_LIBRARIES
    )
endif()

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
add_library(KazDragon::serverpp ALIAS serverpp)

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
        ${SERVERPP_PUBLIC_LIBRARIES}
    PRIVATE
        ${SERVERPP_PRIVATE_LIBRARIES}
)

target_include_directories(serverpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/serverpp-${SERVERPP_VERSION}>
)

set_target_properties(serverpp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION ${SERVERPP_VERSION}
        SOVERSION ${SERVERPP_VERSION}
        DEBUG_POSTFIX "d"
)

target_compile_options(serverpp
    PRIVATE
        # Do not generate warning C4251 (member needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
        # Do not generate warning C4275 (type needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4275>
        # Any warning on MSVC is an error
        $<$<CXX_COMPILER_ID:MSVC>:/WX>

        # Add warnings on g++ and Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wsign-conversion -Werror>
)

# The required C++ Standard for Server++ is at least C++17.
target_compile_features(serverpp PUBLIC cxx_std_17)

generate_export_header(serverpp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/serverpp/detail/export.hpp"
)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/serverpp/version.hpp.in
    ${PROJECT_SOURCE_DIR}/include/serverpp/version.hpp
    @ONLY)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/serverpp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/serverpp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/serverpp
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/serverpp-config-version.cmake"
    VERSION
        "${SERVERPP_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

export(
    TARGETS 
        serverpp
    NAMESPACE 
        KazDragon::
    FILE ${PROJECT_BINARY_DIR}/serverpp-targets.cmake
)

install(
    TARGETS
        serverpp
    EXPORT
        serverpp-targets
    ARCHIVE DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/serverpp-${SERVERPP_VERSION}
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/serverpp-${SERVERPP_VERSION}
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_BINDIR}/serverpp-${SERVERPP_VERSION}
)

install(
    EXPORT
        serverpp-targets
    NAMESPACE 
        KazDragon::
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/serverpp-${SERVERPP_VERSION}
)

install(
    DIRECTORY
        include/
    DESTINATION
        include/serverpp-${SERVERPP_VERSION}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/serverpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/serverpp-config-version.cmake"
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/serverpp-${SERVERPP_VERSION}
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
