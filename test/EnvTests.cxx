#include "ProSelecta/ProSelecta.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"

#include <cassert>

TEST_CASE("Get", "[ps::ProSelecta]") { REQUIRE_NOTHROW(ps::ProSelecta::Get()); }

TEST_CASE("LoadText::no-op", "[ps::ProSelecta]") {
  REQUIRE(ps::ProSelecta::Get().load_text("void no_op(){};",
                                          ps::ProSelecta::Interpreter::kCling));
}
