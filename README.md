# Server++

[![License](https://img.shields.io/github/license/KazDragon/serverpp.svg)](https://en.wikipedia.org/wiki/MIT_License)
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/serverpp.svg)](https://github.com/KazDragon/serverpp/releases)
[![Download](https://api.bintray.com/packages/kazdragon/conan-public/serverpp%3Akazdragon/images/download.svg)](https://bintray.com/kazdragon/conan-public/serverpp%3Akazdragon/_latestVersion)

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

You can also use [The Conan Package Manager](https://conan.io/) to install Server++ and its dependencies.

See [the echo server](example/echo) for a minimalistic project that describes this setup.

# Features / Roadmap / Progress

1. [X] TCP server and sockets.
