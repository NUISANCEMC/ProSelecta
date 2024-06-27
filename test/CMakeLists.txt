CPMAddPackage("gh:catchorg/Catch2@3.3.2")

LIST(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

add_executable(testTests testTests.cxx)
target_link_libraries(testTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(testTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(testTests)

add_executable(eventTests eventTests.cxx)
target_link_libraries(eventTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(eventTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(eventTests)

add_executable(partTests partTests.cxx)
target_link_libraries(partTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(partTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(partTests)

add_executable(vectTests vectTests.cxx)
target_link_libraries(vectTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(vectTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(vectTests)

add_executable(envTests envTests.cxx)
target_link_libraries(envTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(envTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(envTests)

# find_package(Boost 1.70.0)
# if(BOOST_FOUND)

#   add_library(dummyPlugin dummyPlugin.cxx)
#   target_link_libraries(dummyPlugin PRIVATE Boost::boost ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
#   target_include_directories(dummyPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#   add_executable(pluginTests pluginTests.cxx)
#   target_link_libraries(pluginTests PRIVATE Boost::boost Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
#   target_include_directories(pluginTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#   catch_discover_tests(pluginTests)
# endif()