#include "ProSelecta/ProSelecta.h"

#include "HepMC3/GenEvent.h"

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

  REQUIRE(plugin("pluginTest.out.cpp", "dummyfunc") == 13371337);
}

TEST_CASE("LoadFile::via_both", "[ps::ProSelecta]") {

  std::ofstream out("pluginTest.out2.cpp");
  out << "int dummyfunc2(HepMC3::GenEvent const&){ return 13371337; };";
  out.close();

  REQUIRE(ps::ProSelecta::Get().load_file("pluginTest.out2.cpp"));

  auto plugin =
      boost::dll::import_alias<int(std::string const &, std::string const &)>(
          "./libdummyPlugin.so", "load_and_call_select");

  REQUIRE(plugin("pluginTest.out2.cpp", "dummyfunc2") == 13371337);
  HepMC3::GenEvent evt;
  REQUIRE(ps::ProSelecta::Get().get_select_func("dummyfunc2")(evt) == 13371337);
}

TEST_CASE("LoadFile::via_both_pluginfirst", "[ps::ProSelecta]") {

  std::ofstream out("pluginTest.out3.cpp");
  out << "int dummyfunc3(HepMC3::GenEvent const&){ return 13371337; };";
  out.close();

  auto plugin =
      boost::dll::import_alias<int(std::string const &, std::string const &)>(
          "./libdummyPlugin.so", "load_and_call_select");

  REQUIRE(plugin("pluginTest.out3.cpp", "dummyfunc3") == 13371337);
  HepMC3::GenEvent evt;
  REQUIRE(ps::ProSelecta::Get().get_select_func("dummyfunc3")(evt) == 13371337);

  REQUIRE(ps::ProSelecta::Get().load_file("pluginTest.out3.cpp"));
}

TEST_CASE("LoadFile::via_both_filechange", "[ps::ProSelecta]") {

  std::ofstream out("pluginTest.out4.cpp");
  out << "int dummyfunc4(HepMC3::GenEvent const&){ return 13371337; };";
  out.close();

  auto plugin =
      boost::dll::import_alias<int(std::string const &, std::string const &)>(
          "./libdummyPlugin.so", "load_and_call_select");

  REQUIRE(plugin("pluginTest.out4.cpp", "dummyfunc4") == 13371337);

  std::ofstream out2("pluginTest.out4.cpp");
  out2 << "int dummyfunc4(HepMC3::GenEvent const&){ return 13371338; };";
  out2.close();

  REQUIRE(ps::ProSelecta::Get().load_file("pluginTest.out4.cpp"));

  HepMC3::GenEvent evt;
  REQUIRE(ps::ProSelecta::Get().get_select_func("dummyfunc4")(evt) == 13371338);
}
