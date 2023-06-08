add_executable(pyProSelecta pyProSelecta.cxx)

target_link_libraries(pyProSelecta PRIVATE HepMC3::All ${Python3_LIBRARIES})

#use HepMC3 pybind11
target_include_directories(pyProSelecta PRIVATE 
  ${HepMC3_SOURCE_DIR}/python/include 
  ${Python3_INCLUDE_DIRS})

install(TARGETS pyProSelecta DESTINATION bin)

