#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

using namespace ps;

TEST_CASE("project energy", "[ps::part]") {

  auto proton_at_rest = BuildPart("2212 1 0");
  auto fsnu = BuildPart("14 1 1");

  REQUIRE_THAT(energy(proton_at_rest), WithinAbs(0.938 * unit::GeV, 1E-8));
  REQUIRE_THAT(energy(fsnu), WithinAbs(1 * unit::GeV, 1E-8));
}

TEST_CASE("project theta", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(theta(proton), WithinAbs(20 * unit::deg, 1E-8));
  REQUIRE_THAT(theta(proton->momentum())(proton), WithinAbs(0, 1E-8));
  REQUIRE_THAT(theta(HepMC3::FourVector{0, 0, 1, 0})(proton),
               WithinAbs(20 * unit::deg, 1E-8));
  REQUIRE_THAT(theta(HepMC3::FourVector{0, 0, 0, 0})(proton),
               WithinAbs(0, 1E-8));
}

TEST_CASE("project costheta", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(costheta(proton), WithinAbs(std::cos(20 * unit::deg), 1E-8));
  REQUIRE_THAT(costheta(proton->momentum())(proton), WithinAbs(1, 1E-8));
  REQUIRE_THAT(costheta(HepMC3::FourVector{0, 0, 1, 0})(proton),
               WithinAbs(std::cos(20 * unit::deg), 1E-8));
  REQUIRE_THAT(costheta(HepMC3::FourVector{0, 0, 0, 0})(proton),
               WithinAbs(1, 1E-8));
}

TEST_CASE("project momentum", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(momentum(proton).m(), WithinAbs(0.938 * unit::GeV, 1E-8));
  REQUIRE_THAT(momentum(proton).p3mod(), WithinAbs(1 * unit::GeV, 1E-8));
  REQUIRE_THAT(momentum(proton).theta(), WithinAbs(20 * unit::deg, 1E-8));
}

TEST_CASE("p3mod cuts", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE_FALSE((p3mod < 0.5 * unit::GeV)(protons.back()));

  REQUIRE((p3mod < 1 * unit::GeV)(protons.back()));

  REQUIRE((p3mod > 0.25 * unit::GeV)(protons.back()));

  REQUIRE(
      ((p3mod < 1 * unit::GeV) && (p3mod > 0.25 * unit::GeV))(protons.back()));
}

TEST_CASE("sort_ascending p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  auto protons_sorted = part::sort_ascending(p3mod, protons);

  REQUIRE_THAT(protons_sorted.front()->momentum().length(),
               WithinAbs(0.5 * unit::GeV, 1E-8));
  REQUIRE_THAT(protons_sorted.back()->momentum().length(),
               WithinAbs(1.5 * unit::GeV, 1E-8));
}

TEST_CASE("highest/lowest p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE_THAT(part::highest(p3mod, protons)->momentum().length(),
               WithinAbs(1.5 * unit::GeV, 1E-8));
  REQUIRE_THAT(part::lowest(p3mod, protons)->momentum().length(),
               WithinAbs(0.5 * unit::GeV, 1E-8));
}

TEST_CASE("filter p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE(part::filter(p3mod > 2 * unit::GeV, protons).size() == 0);
  REQUIRE(part::filter(p3mod > 1.1 * unit::GeV, protons).size() == 1);
  REQUIRE(part::filter(p3mod > 0.25 * unit::GeV, protons).size() == 3);
  REQUIRE(part::filter(p3mod > 0.75 * unit::GeV && p3mod < 1.25 * unit::GeV,
                       protons)
              .size() == 1);
}

TEST_CASE("sum momentum", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1 45 - 0"), BuildPart("2212 1 1 45 - 120")};
  std::vector<HepMC3::ConstGenParticlePtr> muons{BuildPart("13 1 1 45 - 240")};

  REQUIRE_THAT(
      part::sum(momentum, protons).x(),
      WithinAbs((std::sin(45 * unit::deg) +
                 std::sin(45 * unit::deg) * std::cos(120 * unit::deg)) *
                    unit::GeV,
                1E-8));
  REQUIRE_THAT(part::sum(momentum, std::array{protons, muons}, ps::squeeze).pt(),
               WithinAbs(0, 1E-8));
}

