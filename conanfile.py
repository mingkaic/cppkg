from conans import ConanFile, CMake
import os


class CppkgConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "boost/1.73.0", "grpc/1.29.1@inexorgame/stable", "gtest/1.10.0"
    generators = "cmake", "cmake_find_package_multi"

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
