import os
import sys
import em
from conans import ConanFile, CMake, tools

find_mrt_cmake="""
set(mrt_cmake_modules_FOUND True)
include(${CMAKE_CURRENT_LIST_DIR}/mrt_cmake_modules-extras.cmake)
"""

def read_version():
    with open('version.txt') as f:
        line = next(f)
    return line.rstrip()

def get_py_version():
    return "{}.{}".format(sys.version_info.major, sys.version_info.minor)

class Lanelet2Conan(ConanFile):
    name = "lanelet2"
    version = read_version()
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    license = "BSD"
    url = "https://github.com/fzi-forschungszentrum-informatik/lanelet2"
    description = "Map handling framework for automated driving"
    options = {"shared": [True, False]}
    default_options = {"shared": False} #, "boost_python:python_version": get_py_version()}

    requires = ("boost/1.64.0@conan/stable",
                # "boost_serialization/1.68.0@bincrafters/stable",
                # "boost_geometry/1.68.0@bincrafters/stable",
                # "boost_optional/1.68.0@bincrafters/stable",
                # "boost_variant/1.68.0@bincrafters/stable",
                # "boost_units/1.68.0@bincrafters/stable",
                # "boost_lexical_cast/1.68.0@bincrafters/stable",
                # "boost_filesystem/1.68.0@bincrafters/stable",
                # "boost_property_map/1.68.0@bincrafters/stable",
                # "boost_iterator/1.68.0@bincrafters/stable",
                # "boost_program_options/1.68.0@bincrafters/stable",
                # "boost_python/1.68.0@bincrafters/stable",
                "eigen/3.3.7@conan/stable",
                "geographiclib/1.49@bincrafters/stable",
                "pugixml/1.9@bincrafters/stable")

    exports_sources = "*"
    exports = "version.txt"

    proj_list = [
        'lanelet2_core',
        'lanelet2_io',
        'lanelet2_projection',
        'lanelet2_traffic_rules',
        'lanelet2_routing',
        'lanelet2_validation'
    ]

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["PYTHON_VERSION"] = get_py_version()
        cmake.configure()
        return cmake

    def source(self):
        self.run("git clone https://github.com/KIT-MRT/mrt_cmake_modules.git --branch conan_support")
        mrt_cmake_dir = os.path.join(os.getcwd(), "mrt_cmake_modules")
        with open("mrt_cmake_modules/cmake/mrt_cmake_modules-extras.cmake.em") as f:
            extras = em.expand(f.read(), DEVELSPACE=True, PROJECT_SOURCE_DIR=mrt_cmake_dir,
                               CMAKE_CURRENT_SOURCE_DIR=mrt_cmake_dir)
        with open("mrt_cmake_modules-extras.cmake", "w") as f:
            f.write(extras)
        with open("Findmrt_cmake_modules.cmake", "w") as f:
            f.write(find_mrt_cmake)

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = list(reversed(self.proj_list))
