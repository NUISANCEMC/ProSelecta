pybind11_add_module(pyProSelecta SHARED pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE ProSelecta::Interpreter pybind11::module)

install(TARGETS pyProSelecta
    LIBRARY DESTINATION ${ProSelecta_PYTHONPATH})