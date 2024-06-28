#include "ProSelecta/ProSelecta.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>

TEST_CASE("Get", "[ps::ProSelecta]") { REQUIRE_NOTHROW(ps::ProSelecta::Get()); }

TEST_CASE("LoadText::no-op", "[ps::ProSelecta]") {
  REQUIRE(ps::ProSelecta::Get().load_text("void no_op(){};"));
}

TEST_CASE("LoadFile::no-op", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out1.cpp");
  out << "void no_op1(){};";
  out.close();

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out1.cpp"));
}

TEST_CASE("LoadFile::no-op*2 same file", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out2.cpp");
  out << "void no_op2(){};";
  out.close();

  REQUIRE((ps::ProSelecta::Get().load_file("envTests.out2.cpp") &&
           ps::ProSelecta::Get().load_file("envTests.out2.cpp")));
}

TEST_CASE("LoadFile::get_select_func", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out3.cpp");
  out << "int no_op3(HepMC3::GenEvent const &){ return 1; };";
  out.close();

  HepMC3::GenEvent evt;

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out3.cpp"));
  auto func1 = ps::ProSelecta::Get().get_select_func("no_op3");

  REQUIRE(func1(evt) == 1);
}

TEST_CASE("LoadFile::get_select_func*2", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out4.cpp");
  out << "int no_op4(HepMC3::GenEvent const &){ return 1; };";
  out.close();

  HepMC3::GenEvent evt;

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out4.cpp"));
  auto func1 = ps::ProSelecta::Get().get_select_func("no_op4");
  auto func2 = ps::ProSelecta::Get().get_select_func("no_op4");

  REQUIRE(func1(evt) == 1);
  REQUIRE(func2(evt) == 1);
}

TEST_CASE("LoadFile*2::get_select_func*2", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out5.cpp");
  out << "int no_op5(HepMC3::GenEvent const &){ return 1; };";
  out.close();

  HepMC3::GenEvent evt;

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out5.cpp"));
  auto func1 = ps::ProSelecta::Get().get_select_func("no_op5");

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out5.cpp"));
  auto func2 = ps::ProSelecta::Get().get_select_func("no_op5");

  REQUIRE(func1(evt) == 1);
  REQUIRE(func2(evt) == 1);
}

TEST_CASE("LoadFile::no-op*2 modify file", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out6.cpp");
  out << "int no_op6(HepMC3::GenEvent const &){ return 1; };";
  out.close();

  HepMC3::GenEvent evt;

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out6.cpp"));
  auto func1 = ps::ProSelecta::Get().get_select_func("no_op6");

  REQUIRE(func1(evt) == 1);

  std::ofstream out2("envTests.out6.cpp");
  out2 << "int no_op6(HepMC3::GenEvent const &){ return 2; };";
  out2.close();

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out6.cpp"));
  auto func2 = ps::ProSelecta::Get().get_select_func("no_op6");

  REQUIRE(func2(evt) == 2);
}

TEST_CASE("LoadFile::no-op*2 modify file change type", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out6.cpp");
  out << "int no_op6(HepMC3::GenEvent const &){ return 1; };";
  out.close();

  HepMC3::GenEvent evt;

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out6.cpp"));
  auto func1 = ps::ProSelecta::Get().get_select_func("no_op6");

  REQUIRE(func1(evt) == 1);

  std::ofstream out2("envTests.out6.cpp");
  out2 << "std::vector<int> no_op6(HepMC3::GenEvent const &){ return {3,}; };";
  out2.close();

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out6.cpp"));
  auto func2 = ps::ProSelecta::Get().get_selects_func("no_op6");

  REQUIRE(func2(evt).front() == 3);
}

TEST_CASE("LoadFile::no-op*2 diff file", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out7.cpp");
  out << "void no_op7(){};";
  out.close();

  std::ofstream out2("envTests.out8.cpp");
  out2 << "void no_op7(){};";
  out2.close();

  CHECK_FALSE((ps::ProSelecta::Get().load_file("envTests.out7.cpp") &&
               ps::ProSelecta::Get().load_file("envTests.out8.cpp")));
}

TEST_CASE("LoadFile::path_tests", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out9.cpp");
  out << "void no_op9(){};";
  out.close();

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out9.cpp"));
  REQUIRE(ps::ProSelecta::Get().load_file("./envTests.out9.cpp"));
  REQUIRE(ps::ProSelecta::Get().load_file(".//envTests.out9.cpp"));
  REQUIRE(ps::ProSelecta::Get().load_file(".///envTests.out9.cpp"));
}
