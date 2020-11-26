import os
from conans import ConanFile, CMake

class CppkgConan(ConanFile):
    name = "cppkg"
    version = "0.1"
    license = "MIT"
    author = "Ming Kai Chen <mingkaichen2009@gmail.com>"
    url = "https://github.com/mingkaic/cppkg"
    description = "C++ utility packages."
    topics = ["conan", "utility"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "boost/1.73.0",
        "grpc/1.29.1@inexorgame/stable",
        "gtest/1.10.0",
    )
    generators = "cmake", "cmake_find_package_multi"
    _modules = ["diff", "egrpc", "error", "estd", "exam", "flag", "fmts", "jobs", "logs", "types"]

    def source(self):
        self.run("git clone {}.git".format(self.url))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        for module in self._modules:
            self.copy("*.hpp", dst=os.path.join("include", module), src=module)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["{}_{}".format(self.name, module) for module in _modules]
