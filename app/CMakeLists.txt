add_executable(ProSelectaCPP ProSelectaCPP.cxx)

target_link_libraries(ProSelectaCPP PRIVATE ProSelecta HepMC3::All)

install(TARGETS ProSelectaCPP DESTINATION bin)
