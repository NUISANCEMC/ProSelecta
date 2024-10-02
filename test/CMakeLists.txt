CPMAddPackage("gh:catchorg/Catch2@3.3.2")

LIST(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(Catch)

add_executable(testTests testTests.cxx)
target_link_libraries(testTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(testTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(testTests)

add_executable(eventTests eventTests.cxx)
target_link_libraries(eventTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(eventTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(eventTests)

add_executable(partTests partTests.cxx)
target_link_libraries(partTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(partTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(partTests)

add_executable(vectTests vectTests.cxx)
target_link_libraries(vectTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(vectTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(vectTests)

add_executable(envTests envTests.cxx)
target_link_libraries(envTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(envTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(envTests)

add_executable(READMETests READMETests.cxx)
target_link_libraries(READMETests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(READMETests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(READMETests)

add_executable(unitTests unitTests.cxx)
target_link_libraries(unitTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
target_include_directories(unitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(unitTests)

find_package(Boost 1.70.0 COMPONENTS filesystem)
if(BOOST_FOUND)

  add_library(dummyPlugin SHARED dummyPlugin.cxx)
  target_link_libraries(dummyPlugin PRIVATE Boost::boost ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
  target_include_directories(dummyPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  add_executable(pluginTest pluginTest.cxx)
  target_link_libraries(pluginTest PRIVATE Boost::boost Boost::filesystem Catch2::Catch2WithMain ProSelecta::Interpreter proselecta_private_compile_options ROOT::MathCore HepMC3::All)
  target_include_directories(pluginTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  catch_discover_tests(pluginTest)

  pybind11_add_module(pyProSelectaPlugin SHARED pyProSelectaPlugin.cxx)

  target_link_libraries(pyProSelectaPlugin PRIVATE ProSelecta::Interpreter proselecta_private_compile_options pybind11::module Boost::boost Boost::filesystem)

endif()