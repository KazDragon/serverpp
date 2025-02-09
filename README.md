# Server++

[![License](https://img.shields.io/github/license/KazDragon/serverpp.svg)](https://en.wikipedia.org/wiki/MIT_License)
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/serverpp.svg)](https://github.com/KazDragon/serverpp/releases)
![Build Status](https://github.com/KazDragon/serverpp/workflows/CMake/badge.svg?branch=master)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/b4737922bf834ff79f61019d09cf69e6)](https://www.codacy.com/gh/KazDragon/serverpp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/serverpp&amp;utm_campaign=Badge_Grade)

[![Github Issues](https://img.shields.io/github/issues/KazDragon/serverpp.svg)](https://github.com/KazDragon/serverpp/issues)

# Requirements

Server++ requires a C++17 compiler and the following libraries:
  * Boost (At least version 1.69.0)

# Installation - CMake

Server++ can be installed from source using CMake.  This requires Boost and any other dependencies to have been installed beforehand, using their own instructions, or for the call to `cmake --configure` to be adjusted appropriately (e.g. `-DBOOST_ROOT=...`).  If you do not wish to install into a system directory, and thus avoid the use of sudo, you can also pass `-DCMAKE_INSTALL_PREFIX=...` into the `cmake --configure` call.

    git clone https://github.com/KazDragon/serverpp.git && cd serverpp
    mkdir build && cd build
    cmake --configure -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    sudo cmake --install .

# Features / Roadmap / Progress

1. [X] TCP server and sockets.
