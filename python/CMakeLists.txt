pybind11_add_module(pyProSelecta SHARED pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE ProSelecta::Interpreter pybind11::module)
#PS Hacking include for pyProSelecta for now whilst structure sorted.
target_include_directories(pyProSelecta PRIVATE ${Python3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../env/)

install(TARGETS pyProSelecta
    LIBRARY DESTINATION ${ProSelecta_PYTHONPATH})