# Server++

[![License](https://img.shields.io/github/license/KazDragon/serverpp.svg)](https://en.wikipedia.org/wiki/MIT_License)
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/serverpp.svg)](https://github.com/KazDragon/serverpp/releases)
![Build Status](https://github.com/KazDragon/serverpp/workflows/build-master-system/badge.svg?branch=master)
[![Download](https://api.bintray.com/packages/kazdragon/conan-public/serverpp%3Akazdragon/images/download.svg)](https://bintray.com/kazdragon/conan-public/serverpp%3Akazdragon/_latestVersion)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/b4737922bf834ff79f61019d09cf69e6)](https://www.codacy.com/gh/KazDragon/serverpp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/serverpp&amp;utm_campaign=Badge_Grade)

[![Github Issues](https://img.shields.io/github/issues/KazDragon/serverpp.svg)](https://github.com/KazDragon/serverpp/issues)

Server++ is an implementation of a basic networking server such as might be used for the connection handling of a Telnet server.

# Requirements

Server++ requires a C++14 compiler and the following libraries:
  * Boost (At least version 1.69.0)
  * GSL-lite (Exactly version 1.34)

# Installation - CMake

Server++ can be installed from source using CMake.  This requires Boost, GSL-Lite and any other dependencies to have been installed beforehand, using their own instructions, or for the call to `cmake --configure` to be adjusted appropriately (e.g. `-DBOOST_ROOT=...` or `-Dgsl-lite_DIR=...`).  If you do not wish to install into a system directory, and thus avoid the use of sudo, you can also pass `-DCMAKE_INSTALL_PREFIX=...` into the `cmake --configure` call.

    git clone https://github.com/KazDragon/serverpp.git && cd serverpp
    mkdir build && cd build
    cmake --configure -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    sudo cmake --install .

# Installation - Conan

You can also use [The Conan Package Manager](https://conan.io/) to build and/or install Server++ and its dependencies.

See [the echo server](example/echo) for a minimalistic project that uses Conan in order to fetch Server++ as a dependency.  Alternatively, [the Travis script](.travis.yml) for continuous integration also verifies that Server++ itself can be built using Conan.

# Features / Roadmap / Progress

1. [X] TCP server and sockets.
