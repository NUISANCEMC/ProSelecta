set(HEADERS 
  ftypes.h 
  ProSelecta.h)

add_library(ProSelecta SHARED ProSelecta.cxx)

target_link_libraries(ProSelecta PRIVATE 
  HepMC3::All 
  ROOT::Core
  pybind11::embed)

set_target_properties(ProSelecta PROPERTIES 
  PUBLIC_HEADER "${HEADERS}")

target_include_directories(ProSelecta PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>)

target_include_directories(ProSelecta PRIVATE
  ${Python3_INCLUDE_DIRS})

install(TARGETS ProSelecta 
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

install(DIRECTORY env DESTINATION include/ProSelecta)

# Python bindings

pybind11_add_module(pyProSelecta SHARED pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE 
  ProSelecta)

target_include_directories(pyProSelecta PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>)

install(TARGETS pyProSelecta
    LIBRARY DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/__init__.py.in ${CMAKE_CURRENT_BINARY_DIR}/__init__.py @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)