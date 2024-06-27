#include "ProSelecta/ProSelecta.h"

#include "boost/dll/import.hpp"

#include "catch2/catch_test_macros.hpp"

#include <cassert>
#include <fstream>

TEST_CASE("LoadFile::via_plugin", "[ps::ProSelecta]") {

  std::ofstream out("pluginTest.out.cpp");
  out << "int dummyfunc(HepMC3::GenEvent const&){ return 13371337; };";
  out.close();

  auto plugin =
      boost::dll::import_alias<int(std::string const &, std::string const &)>(
          "./libdummyPlugin.so", "load_and_call_select");

  REQUIRE(ps::ProSelecta::Get().load_text("void no_op(){};"));

  REQUIRE(plugin("pluginTest.out.cpp", "dummyfunc") == 13371337);
}
