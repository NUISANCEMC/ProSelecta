add_executable(ProSelectaCPP ProSelectaCPP.cxx)

target_link_libraries(ProSelectaCPP PRIVATE ProSelecta::Interpreter)

install(TARGETS ProSelectaCPP DESTINATION bin)
