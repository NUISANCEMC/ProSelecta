cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

SET(ProSelecta_VERSION 0.1.2)
project(ProSelecta VERSION ${ProSelecta_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(ProSelecta_ENABLE_TESTS "Whether to enable test suite" OFF)
option(ProSelecta_ENABLE_SANITIZERS "Whether to enable ASAN LSAN and UBSAN" OFF)
option(ProSelecta_ENABLE_GCOV "Whether to enable GCOV" OFF)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/"
           CACHE PATH "default install path" FORCE)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  if(ProSelecta_ENABLE_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
  else()
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
  endif()
endif()

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules)

######################### Dependencies

add_library(proselecta_private_compile_options INTERFACE)
target_compile_options(proselecta_private_compile_options INTERFACE -Wall -Werror -Wextra)
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    target_compile_options(proselecta_private_compile_options INTERFACE -Wno-class-memaccess)
endif()

if(ProSelecta_ENABLE_SANITIZERS)
  target_compile_options(proselecta_private_compile_options BEFORE INTERFACE -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined)
  target_link_options(proselecta_private_compile_options BEFORE INTERFACE -fsanitize=address -fsanitize=leak -fsanitize=undefined)
endif()

if(ProSelecta_ENABLE_GCOV)
  target_compile_options(proselecta_private_compile_options BEFORE INTERFACE -fprofile-arcs -ftest-coverage)
  target_link_options(proselecta_private_compile_options BEFORE INTERFACE -fprofile-arcs -ftest-coverage)
endif()

set(Python3_MIN_VERSION 3.7)
set(ROOT_MIN_VERSION 6.10)
set(pybind11_MIN_VERSION 2.13.6)

find_package(Python3 ${Python3_MIN_VERSION} REQUIRED COMPONENTS Interpreter Development)
find_package(ROOT ${ROOT_MIN_VERSION} REQUIRED)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
include(get_cpm)

find_package(Protobuf 2.4)
find_package(HepMC3 REQUIRED)

if(TARGET HepMC3::HepMC3)
  get_target_property(ProSelecta_HEPMC3_INCLUDE_DIR HepMC3::HepMC3 INTERFACE_INCLUDE_DIRECTORIES)
elseif(DEFINED HEPMC3_INCLUDE_DIR AND NOT "${HEPMC3_INCLUDE_DIR}x" STREQUAL "x")
  set(ProSelecta_HEPMC3_INCLUDE_DIR ${HEPMC3_INCLUDE_DIR})
else()
  message(FATAL_ERROR "Could not determine value for ProSelecta_HEPMC3_INCLUDE_DIR required for ProSelecta runtime installation.")
endif()

message("ProSelecta_HEPMC3_INCLUDE_DIR: ${ProSelecta_HEPMC3_INCLUDE_DIR}")

add_subdirectory(src/ProSelecta)
add_subdirectory(app)

CPMFindPackage(
    NAME pybind11
    VERSION ${pybind11_MIN_VERSION}
    GITHUB_REPOSITORY pybind/pybind11
    GIT_TAG "v${pybind11_MIN_VERSION}"
)
set(ProSelecta_PYSITEARCH "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
set(ProSelecta_PYTHONPATH "python/${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

add_subdirectory(python)

if(ProSelecta_ENABLE_TESTS)

  CPMAddPackage(
      NAME Catch2
      GITHUB_REPOSITORY catchorg/Catch2
      VERSION 3.3.2
  )

  LIST(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

  include(CTest)
  include(Catch)

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

install(DIRECTORY env/ProSelecta DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
