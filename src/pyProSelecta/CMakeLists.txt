pybind11_add_module(pyProSelecta SHARED pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE 
  ProSelecta::Interpreter HepMC3::All)

target_include_directories(pyProSelecta PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>)

install(TARGETS pyProSelecta
    LIBRARY DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/__init__.py.in 
  ${CMAKE_CURRENT_BINARY_DIR}/__init__.py @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py 
  DESTINATION ${ProSelecta_PYTHONPATH}/pyProSelecta)
