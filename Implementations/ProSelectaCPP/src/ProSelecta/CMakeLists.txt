set(HEADERS 
  ftypes.h 
  ProSelecta.h)

add_library(ProSelecta SHARED ProSelecta.cxx)

target_link_libraries(ProSelecta PRIVATE pybind11:embed HepMC3_static::All ROOT::Core)

set_target_properties(ProSelecta PROPERTIES 
  PUBLIC_HEADER "${HEADERS}")

target_include_directories(ProSelecta PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>)

install(TARGETS ProSelecta 
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

install(DIRECTORY env DESTINATION include/ProSelecta)