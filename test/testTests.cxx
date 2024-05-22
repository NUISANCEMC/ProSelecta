#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

TEST_CASE("BuildMinPart", "[test-tools]") {
  auto part = BuildPart("2212 11 1");
  REQUIRE(part->pid() == 2212);
  REQUIRE(part->status() == 11);
  REQUIRE_THAT(part->momentum().length(), WithinAbs(1 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().m(), WithinAbs(0.938 * ps::unit::GeV, 1E-8));
}

TEST_CASE("BuildPartSkipMass", "[test-tools]") {
  auto part = BuildPart("2212 11 1 30 - 0");
  REQUIRE_THAT(part->momentum().m(), WithinAbs(0.938 * ps::unit::GeV, 1E-8));

  REQUIRE_THAT(part->momentum().theta() * (180.0 / M_PI), WithinAbs(30, 1E-8));
  REQUIRE_THAT(part->momentum().x(), WithinAbs(0.5 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().y(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(part->momentum().z(),
               WithinAbs(std::sqrt(1 - std::pow(.5, 2)) * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().m(), WithinAbs(.938 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().phi() * (180.0 / M_PI), WithinAbs(0, 1E-8));
}

TEST_CASE("BuildFullPart", "[test-tools]") {
  auto part = BuildPart("2212 11 1 20 .937 45");
  REQUIRE(part->pid() == 2212);
  REQUIRE(part->status() == 11);
  REQUIRE_THAT(part->momentum().length(), WithinAbs(1 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().theta() * (180.0 / M_PI), WithinAbs(20, 1E-8));
  REQUIRE_THAT(part->momentum().m(), WithinAbs(0.937 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(part->momentum().phi() * (180.0 / M_PI), WithinAbs(45, 1E-8));
}

TEST_CASE("BuildEvtSingleVertex", "[test-tools]") {
  auto evt1 = BuildEvent(
      {{"14 4 1 0", "2112 11 0"}, {"13 1 0.7 5 - 30", "2212 1 0.15 5 - -30"}});
  REQUIRE(evt1.particles().size() == 4);
  REQUIRE(evt1.vertices().size() == 1);
  REQUIRE(evt1.vertices()[0]->particles_in()[0]->pid() == 14);
  REQUIRE(evt1.vertices()[0]->particles_in()[1]->pid() == 2112);
  REQUIRE(evt1.vertices()[0]->particles_out()[0]->pid() == 13);
  REQUIRE(evt1.vertices()[0]->particles_out()[1]->pid() == 2212);
  REQUIRE(evt1.vertices()[0]->status() == 0);

  auto evt2 = BuildEvent(
      {{"14 4 1 0", "2112 11 0"}, {"13 1 0.7 5 - 30", "2212 1 0.15 5 - -30"}},
      {1});
  REQUIRE(evt2.vertices()[0]->status() == 1);
}

TEST_CASE("BuildEvt", "[test-tools]") {
  auto evt1 = BuildEvent({{"14 4 1 0", "1000060120 11 0"},
                          {"14 3 1 0", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7 5 - 30", "2212 1 0.15 5 - -30"}},
                         {2, 1});

  int inc = 0;
  int out = 0;

  for (auto const &p : evt1.particles()) {
    if ((p->status() == 4) || (p->status() == 11)) {
      inc++;
    } else if (p->status() == 1) {
      out++;
    }
  }

  REQUIRE(inc == 2);
  REQUIRE(out == 3);
}