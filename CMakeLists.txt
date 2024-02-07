cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

SET(ProSelecta_VERSION 0.1)
project(ProSelecta VERSION ${ProSelecta_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/"
           CACHE PATH "default install path" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules)

#### RPATH handling #####

set(CMAKE_MACOSX_RPATH 1)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
ENDIF("${isSystemDir}" STREQUAL "-1")

######################### Dependencies

set(Python3_MIN_VERSION 3.6)
set(ROOT_MIN_VERSION 6.10)
set(pybind11_MIN_VERSION 2.9.2)

find_package(Python3 ${Python3_MIN_VERSION} REQUIRED COMPONENTS Interpreter Development)
find_package(ROOT ${ROOT_MIN_VERSION} REQUIRED)

set(ProSelecta_PYSITEARCH "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
set(ProSelecta_PYTHONPATH "python/${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
include(CPM)

CPMFindPackage(
    NAME NuHepMC_CPPUtils
    GIT_TAG main
    GIT_REPOSITORY "https://github.com/NuHepMC/cpputils.git"
    OPTIONS "BUILTIN_HEPMC3 ON"
)

add_subdirectory(src/ProSelecta)
add_subdirectory(app)
if(ProSelecta_TESTS_ENABLED)
  add_subdirectory(test)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Templates/setup.ProSelecta.sh.in 
  ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.ProSelecta.sh DESTINATION bin)


install(EXPORT proselecta-targets
  FILE ProSelectaTargets.cmake
  NAMESPACE ProSelecta::
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/ProSelecta
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Templates/ProSelectaConfig.cmake.in 
  ${CMAKE_CURRENT_BINARY_DIR}/ProSelectaConfig.cmake
  INSTALL_DESTINATION 
    /this/is/ignored/for/some/reason/thanks/kitware
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ProSelectaConfigVersion.cmake
  VERSION ${ProSelecta_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/ProSelectaConfig.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/ProSelectaConfigVersion.cmake 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/ProSelecta)

