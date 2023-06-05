set(HEADERS ftypes.h ProSelecta.h Selectors.h Units.h)

add_library(ProSelecta SHARED ProSelecta.cxx)

target_link_libraries(ProSelecta PUBLIC HepMC3::All ROOT::Core)

set_target_properties(ProSelecta PROPERTIES 
  PUBLIC_HEADER "${HEADERS}")

target_include_directories(ProSelecta PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<INSTALL_INTERFACE:include>)

install(TARGETS ProSelecta 
    EXPORT proselecta-targets
    LIBRARY DESTINATION lib/
    PUBLIC_HEADER DESTINATION include/ProSelecta)

install(DIRECTORY detail DESTINATION include/ProSelecta)