#include "ProSelecta/ProSelecta.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"

#include <cassert>
#include <fstream>

TEST_CASE("Get", "[ps::ProSelecta]") { REQUIRE_NOTHROW(ps::ProSelecta::Get()); }

TEST_CASE("LoadText::no-op", "[ps::ProSelecta]") {
  REQUIRE(ps::ProSelecta::Get().load_text("void no_op(){};"));
}

TEST_CASE("LoadFile::no-op", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out1.cpp");
  out << "void no_op(){};";
  out.close();

  REQUIRE(ps::ProSelecta::Get().load_file("envTests.out1.cpp"));
}

TEST_CASE("LoadFile::no-op*2 same file", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out2.cpp");
  out << "void no_op(){};";
  out.close();

  REQUIRE((ps::ProSelecta::Get().load_file("envTests.out2.cpp") &&
           ps::ProSelecta::Get().load_file("envTests.out2.cpp")));
}

TEST_CASE("LoadFile::no-op*2 diff file", "[ps::ProSelecta]") {

  std::ofstream out("envTests.out3.cpp");
  out << "void no_op(){};";
  out.close();

  std::ofstream out2("envTests.out4.cpp");
  out2 << "void no_op(){};";
  out2.close();

  CHECK_FALSE((ps::ProSelecta::Get().load_file("envTests.out3.cpp") &&
               ps::ProSelecta::Get().load_file("envTests.out4.cpp")));
}
