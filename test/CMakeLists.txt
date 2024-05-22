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

add_executable(EnvTests EnvTests.cxx)
target_link_libraries(EnvTests PRIVATE Catch2::Catch2WithMain ProSelecta::Interpreter ROOT::MathCore HepMC3::All)
target_include_directories(EnvTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(EnvTests)