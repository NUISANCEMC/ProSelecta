add_executable(ProSelectaCPP ProSelectaCPP.cxx)

target_link_libraries(ProSelectaCPP PRIVATE ProSelecta::Interpreter proselecta_private_compile_options)

install(TARGETS ProSelectaCPP DESTINATION bin)
