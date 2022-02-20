from conans import ConanFile, CMake, tools

def get_version():
    try:
        content = tools.load("VERSION")
        return content.strip()
    except Exception as e:
        return None

class CppkgConan(ConanFile):
    name = "cppkg"
    version = get_version()
    license = "MIT"
    author = "Ming Kai Chen <mingkaichen2009@gmail.com>"
    url = "https://github.com/mingkaic/cppkg"
    description = "C++ utility packages."
    topics = ["conan", "utility"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "boost/1.73.0",
        "cisab/0.1.0@mingkaic-co/stable",
        "verum/0.1.1@mingkaic-co/stable",
        "grpc/1.43.0"
    )
    generators = "cmake", "cmake_find_package_multi"

    options = {
        "fPIC": [True, False],
    }
    default_options = {
        "fPIC": True,
    }

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.fPIC
        cmake.configure()
        return cmake

    def configure(self):
        if self.settings.os == "Windows" and self.settings.compiler == "Visual Studio":
            del self.options.fPIC
            compiler_version = tools.Version(self.settings.compiler.version)
            if compiler_version < 14:
                raise ConanInvalidConfiguration("gRPC can only be built with Visual Studio 2015 or higher.")

    def source(self):
        self.run("git clone {}.git .".format(self.url))

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.*", dst="licenses", keep_path=False)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = self.name
        self.cpp_info.names["cmake_find_package_multi"] = self.name
        self.cpp_info.libs = [
            "arrs",
            "diff",
            "egrpc",
            "error",
            "estd",
            "flag",
            "jobs",
            "numbers",
            "units"
        ]
