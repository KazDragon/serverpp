A program that showcases the basic functionality of Server++ by implementing a server that echoes all input back to its client.

To build: you will need the Conan remotes that are registered with the following commands:

```bash
conan remote add conan-center https://conan.bintray.com
conan remote add kazdragon https://kazdragon.jfrog.io/artifactory/api/conan/conan-public
```

The first remote (conan-center) should be there by default.  You can see this by running `conan remote list`.

Conan packages are then installed as following (assuming you are in the examples/echo folder):

```bash
conan install -s compiler.libcxx=libstdc++11 -s cppstd=14 --build=missing .
```

You can then build with:

```bash
cmake .
make -j
```

```bash
./echo server
```

You should see a message saying that the server is started up on a particular port.  You can view the results by connecting to that port number with your favourite Telnet client, and by typing any text you like.
