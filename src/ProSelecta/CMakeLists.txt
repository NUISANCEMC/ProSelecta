set(HEADERS 
  ftypes.h 
  ProSelecta.h)

add_library(ProSelecta SHARED ProSelecta.cxx)

target_link_libraries(ProSelecta PRIVATE 
  HepMC3::All 
  ROOT::Core
  pybind11::embed)

include_directories(HepMC3::All)
include_directories(ROOT::Core)

set_target_properties(ProSelecta PROPERTIES 
  PUBLIC_HEADER "${HEADERS}")

target_include_directories(ProSelecta PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>
  HepMC3::All 
  ROOT::Core
)

target_include_directories(ProSelecta PRIVATE
  ${Python3_INCLUDE_DIRS})

install(TARGETS ProSelecta 
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

install(DIRECTORY env DESTINATION include/ProSelecta)

add_custom_command(
         TARGET ProSelecta POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E create_symlink              
             ${CMAKE_SOURCE_DIR}/src/ProSelecta ProSelecta
         COMMENT "Creating symlink to ProSelecta Env directory" VERBATIM
         )



# Python bindings

pybind11_add_module(pyProSelecta SHARED pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE 
  ProSelecta HepMC3::All)

target_include_directories(pyProSelecta PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>)

install(TARGETS pyProSelecta
    LIBRARY DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/__init__.py.in ${CMAKE_CURRENT_BINARY_DIR}/__init__.py @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)
