cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

SET(ProSelecta_VERSION 0.1)
project(ProSelecta VERSION ${ProSelecta_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/"
           CACHE PATH "default install path" FORCE)
endif()

set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(Python3_MIN_VERSION 3.6)
set(ROOT_MIN_VERSION 6.10)
set(pybind11_MIN_VERSION 2.9.2)

find_package(Python3 ${Python3_MIN_VERSION} REQUIRED COMPONENTS Interpreter Development)
find_package(ROOT ${ROOT_MIN_VERSION} REQUIRED)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

set(ProSelecta_PYSITEARCH "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
set(ProSelecta_PYTHONPATH "python/${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

include(CPM)

CPMAddPackage(
    NAME NuHepMC_CPPUtils
    GIT_TAG main
    GIT_REPOSITORY "https://github.com/NuHepMC/cpputils.git"
    OPTIONS "BUILTIN_HEPMC3 ON"
)

#get the same version of pybind11 as HepMC3 is using
CPMFindPackage(
    NAME pybind11
    VERSION ${pybind11_MIN_VERSION}
    GITHUB_REPOSITORY pybind/pybind11
    GIT_TAG "v${pybind11_MIN_VERSION}"
)

add_subdirectory(src/ProSelecta)
# add_subdirectory(src/pyProSelecta)
add_subdirectory(app)
#add_subdirectory(test)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/setup.ProSelecta.sh.in 
  ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh DESTINATION bin)


install(EXPORT proselecta-targets
  FILE ProSelectaTargets.cmake
  NAMESPACE ProSelecta::
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/ProSelecta
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/ProSelectaConfig.cmake.in 
  ${CMAKE_BINARY_DIR}/ProSelectaConfig.cmake
  INSTALL_DESTINATION 
    /this/is/ignored/for/some/reason/thanks/kitware
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/ProSelectaConfigVersion.cmake
  VERSION ${ProSelecta_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES 
    ${CMAKE_BINARY_DIR}/ProSelectaConfig.cmake 
    ${CMAKE_BINARY_DIR}/ProSelectaConfigVersion.cmake 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/ProSelecta)

