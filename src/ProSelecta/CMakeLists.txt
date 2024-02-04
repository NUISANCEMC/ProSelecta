set(HEADERS 
  ftypes.h 
  ProSelecta.h)

add_library(ProSelectaInterpreter SHARED ProSelecta.cxx)

target_link_libraries(ProSelectaInterpreter PUBLIC 
  NuHepMC::CPPUtils
  ROOT::Core ROOT::Hist
  pybind11::embed)

include_directories(HepMC3::All)
include_directories(ROOT::Core)

set_target_properties(ProSelectaInterpreter PROPERTIES 
  PUBLIC_HEADER "${HEADERS}"
  EXPORT_NAME Interpreter)

target_include_directories(ProSelectaInterpreter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(ProSelectaInterpreter PRIVATE
  ${Python3_INCLUDE_DIRS})

install(TARGETS ProSelectaInterpreter
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

add_library(ProSelecta::Interpreter ALIAS ProSelectaInterpreter)

install(DIRECTORY env DESTINATION include/ProSelecta)

# add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND cp -r ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/ENV
# )