add_executable(FunctionTests FunctionTests.cxx)
target_link_libraries(FunctionTests PRIVATE ProSelecta ROOT::MathCore HepMC3::All)
target_include_directories(FunctionTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})