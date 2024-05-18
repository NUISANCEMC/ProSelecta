#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

TEST_CASE("momentum cuts", "[ps::event]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE(!(ps::momentum < 0.5)(protons.back()));
  REQUIRE((ps::momentum <= 0.5)(protons.back()));
  REQUIRE(!(ps::momentum > 0.5)(protons.back()));
  REQUIRE((ps::momentum >= 0.5)(protons.back()));

  REQUIRE((ps::momentum < 1)(protons.back()));
  REQUIRE((ps::momentum <= 1)(protons.back()));

  REQUIRE((ps::momentum > 0.25)(protons.back()));
  REQUIRE((ps::momentum >= 0.25)(protons.back()));

  REQUIRE(((ps::momentum < 1) && (ps::momentum > 0.25))(protons.back()));
}

TEST_CASE("sortascendingby momentum", "[ps::event]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  auto protons_sorted = ps::part::sortascendingby(ps::momentum, protons);

  REQUIRE_THAT(protons_sorted.front()->momentum().length(),
               WithinAbs(0.5, 1E-8));
  REQUIRE_THAT(protons_sorted.back()->momentum().length(),
               WithinAbs(1.5, 1E-8));
}

TEST_CASE("highest/lowest momentum", "[ps::event]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE_THAT(ps::part::highest(ps::momentum, protons)->momentum().length(),
               WithinAbs(1.5, 1E-8));
  REQUIRE_THAT(ps::part::lowest(ps::momentum, protons)->momentum().length(),
               WithinAbs(0.5, 1E-8));
}

TEST_CASE("filter momentum", "[ps::event]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE(ps::part::filter(ps::momentum > 2, protons).size() == 0);
  REQUIRE(ps::part::filter(ps::momentum > 1, protons).size() == 1);
  REQUIRE(ps::part::filter(ps::momentum >= 1, protons).size() == 2);
  REQUIRE(ps::part::filter(ps::momentum > 0.25, protons).size() == 3);
  REQUIRE(ps::part::filter(ps::momentum > 0.75 && ps::momentum < 1.25, protons)
              .size() == 1);
}
