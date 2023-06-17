cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

project(ProSelecta VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

find_package(Python3 3.6 REQUIRED COMPONENTS Interpreter Development)
find_package(ROOT 6.10 REQUIRED)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

set(ProSelecta_PYSITEARCH "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
set(ProSelecta_PYTHONPATH "python/${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

include(CPM)
CPMAddPackage(
    NAME HepMC3
    VERSION 3.2.6
    GIT_REPOSITORY "https://gitlab.cern.ch/hepmc/HepMC3.git"
    GIT_TAG 3.2.6
    OPTIONS
      "HEPMC3_CXX_STANDARD ${CMAKE_CXX_STANDARD}"
      "HEPMC3_ENABLE_SEARCH OFF"
      "HEPMC3_ENABLE_ROOTIO OFF"
      "HEPMC3_ENABLE_PROTOBUFIO OFF"
      "HEPMC3_ENABLE_PYTHON ON"
      "HEPMC3_Python_SITEARCH${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} ${CMAKE_INSTALL_PREFIX}/${ProSelecta_PYTHONPATH}"
      "HEPMC3_BUILD_DOCS OFF"
      "HEPMC3_BUILD_EXAMPLES OFF"
      "HEPMC3_INSTALL_EXAMPLES OFF"
      "HEPMC3_ENABLE_TEST OFF"
      "HEPMC3_INSTALL_INTERFACES OFF"
      "HEPMC3_BUILD_STATIC_LIBS ON"
)

#get the same version of pybind11 as HepMC3 is using
CPMAddPackage(
    NAME pybind11
    VERSION 2.9.2
    GITHUB_REPOSITORY pybind/pybind11
    GIT_TAG v2.9.2
)

add_subdirectory(src/ProSelecta)
add_subdirectory(app)
add_subdirectory(test)