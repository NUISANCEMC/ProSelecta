cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

project(ProSelecta VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/"
           CACHE PATH "default install path" FORCE)
endif()
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Python3 3.6 REQUIRED COMPONENTS Interpreter Development)
find_package(ROOT 6.10 REQUIRED)

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

#find_package(HepMC3 REQUIRED)
add_library(HepMC3_includes INTERFACE)
target_include_directories(HepMC3_includes SYSTEM INTERFACE ${HepMC3_SOURCE_DIR}/include)

#get the same version of pybind11 as HepMC3 is using
CPMFindPackage(
    NAME pybind11
    VERSION 2.9.2
    GITHUB_REPOSITORY pybind/pybind11
    GIT_TAG v2.9.2
)

find_package(pybind11 REQUIRED)

add_subdirectory(src/ProSelecta)
add_subdirectory(app)
#add_subdirectory(test)

#install(FILES
#  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.ProSelecta.sh" 
#  DESTINATION
#  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/bin/)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/__init__.py.in ${CMAKE_CURRENT_BINARY_DIR}/__init__.py @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/setup.ProSelecta.sh.in ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/ProSelectaConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/ProSelectaConfig.cmake @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/ProSelectaConfig.cmake DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh DESTINATION bin)

#file(CREATE_LINK ${PROJECT_SOURCE_DIR}/src/ProSelecta ProSelecta)
