set(HEADERS 
  FuncTypes.h
  ProSelecta.h
  ProSelecta_cling.h)

add_library(ProSelectaInterpreter SHARED ProSelecta.cxx ProSelecta_cling.cxx)

target_link_libraries(ProSelectaInterpreter PUBLIC 
  HepMC3::HepMC3
  ROOT::Core)
target_link_libraries(ProSelectaInterpreter PRIVATE proselecta_private_compile_options)

set_target_properties(ProSelectaInterpreter PROPERTIES 
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME Interpreter)

target_compile_options(ProSelectaInterpreter PUBLIC -Wno-psabi)

target_include_directories(ProSelectaInterpreter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../env>
  $<INSTALL_INTERFACE:include>
  $<INSTALL_INTERFACE:env>
)

install(TARGETS ProSelectaInterpreter
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

add_library(ProSelecta::Interpreter ALIAS ProSelectaInterpreter)