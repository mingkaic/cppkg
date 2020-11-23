from conans import ConanFile, CMake
import os


class CppkgConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "boost/1.73.0", "grpc/1.29.1@inexorgame/stable"
    generators = "cmake", "cmake_find_package_multi"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
