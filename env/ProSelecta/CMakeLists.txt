set(HEADERS 
  FuncTypes.h
  ProSelecta.h
  ProSelecta_cling.h)

add_library(ProSelectaInterpreter SHARED ProSelecta.cxx ProSelecta_cling.cxx)

target_link_libraries(ProSelectaInterpreter PUBLIC 
  NuHepMC::CPPUtils
  ROOT::Core ROOT::Hist)

set_target_properties(ProSelectaInterpreter PROPERTIES 
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME Interpreter)

target_include_directories(ProSelectaInterpreter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS ProSelectaInterpreter
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

add_library(ProSelecta::Interpreter ALIAS ProSelectaInterpreter)

install(DIRECTORY env DESTINATION include/ProSelecta)