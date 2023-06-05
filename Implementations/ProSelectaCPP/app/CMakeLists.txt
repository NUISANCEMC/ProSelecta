add_executable(ProSelectaCPP ProSelectaCPP.cxx)

target_link_libraries(ProSelectaCPP PRIVATE ProSelecta)

install(TARGETS ProSelectaCPP DESTINATION bin)

add_executable(ProSelectaCPPUP ProSelecta_UnlimitedPower.cxx)
target_link_libraries(ProSelectaCPPUP PRIVATE ProSelecta)
install(TARGETS ProSelectaCPPUP DESTINATION bin)
