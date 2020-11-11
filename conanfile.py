from conans import ConanFile
from conans import CMake

class ConanServerpp(ConanFile):
    name = "serverpp"
    version = "0.0.5"
    url = "https://github.com/KazDragon/serverpp"
    author = "KazDragon"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*.hpp", "*.in", "*.cpp", "CMakeLists.txt", "*.md", "LICENSE"
    description = "A library for a simple networking server"
    requires = ("boost/[>=1.69]", "gsl-lite/[=0.34]")
    options = {"shared": [True, False], "coverage": [True, False], "sanitize" : ["off", "address"]}
    default_options = {"shared": False, "coverage": False, "sanitize": "off"}

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        cmake.definitions["SERVERPP_COVERAGE"] = self.options.coverage
        cmake.definitions["SERVERPP_SANITIZE"] = self.options.sanitize
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.lib", dst="lib", src="lib", keep_path=False)
        self.copy("*.dll", dst="bin", src="bin", keep_path=False)
        self.copy("*.dylib", dst="bin", src="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.so.*", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["serverpp"]