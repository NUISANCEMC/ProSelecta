CPMAddPackage("gh:catchorg/Catch2@3.3.2")

LIST(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

add_executable(FunctionTests FunctionTests.cxx)
target_link_libraries(FunctionTests PRIVATE Catch2::Catch2WithMain ProSelecta ROOT::MathCore HepMC3::All)
target_include_directories(FunctionTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

catch_discover_tests(FunctionTests)