#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

using namespace ps;

using namespace ps::unit;

TEST_CASE("project energy", "[ps::part]") {

  auto proton_at_rest = BuildPart("2212 1 0");
  auto fsnu = BuildPart("14 1 1");

  REQUIRE_THAT(energy(proton_at_rest), WithinAbs(0.938_GeV_c, 1E-8));
  REQUIRE_THAT(energy(fsnu), WithinAbs(1_GeV_c, 1E-8));
}

TEST_CASE("project theta", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(theta(proton), WithinAbs(20_deg, 1E-8));
  REQUIRE_THAT(theta(proton->momentum())(proton), WithinAbs(0, 1E-8));
  REQUIRE_THAT(theta(HepMC3::FourVector{0, 0, 1, 0})(proton),
               WithinAbs(20_deg, 1E-8));
  REQUIRE_THAT(theta(HepMC3::FourVector{0, 0, 0, 0})(proton),
               WithinAbs(0, 1E-8));
}

TEST_CASE("project costheta", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(costheta(proton), WithinAbs(std::cos(20_deg), 1E-8));
  REQUIRE_THAT(costheta(proton->momentum())(proton), WithinAbs(1, 1E-8));
  REQUIRE_THAT(costheta(HepMC3::FourVector{0, 0, 1, 0})(proton),
               WithinAbs(std::cos(20_deg), 1E-8));
  REQUIRE_THAT(costheta(HepMC3::FourVector{0, 0, 0, 0})(proton),
               WithinAbs(1, 1E-8));
}

TEST_CASE("project momentum", "[ps::part]") {

  auto proton = BuildPart("2212 1 1 20 - 45");

  REQUIRE_THAT(momentum(proton).m(), WithinAbs(0.938_GeV_c, 1E-8));
  REQUIRE_THAT(momentum(proton).p3mod(), WithinAbs(1_GeV_c, 1E-8));
  REQUIRE_THAT(momentum(proton).theta(), WithinAbs(20_deg, 1E-8));
}

TEST_CASE("p3mod cuts", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1 20 - 45"),
      BuildPart("2212 1 0.5")};

  REQUIRE_FALSE((p3mod < 0.5_GeV_c)(protons.back()));

  REQUIRE((p3mod < 1_GeV_c)(protons.back()));

  REQUIRE((p3mod > 0.25_GeV_c)(protons.back()));

  REQUIRE(((p3mod < 1_GeV_c) && (p3mod > 0.25_GeV_c))(protons.back()));

  REQUIRE_FALSE(((p3mod > 1_GeV_c) && (p3mod > 0.25_GeV_c))(protons.back()));
}

TEST_CASE("p3mod filter", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1 20 - 45"),
      BuildPart("2212 1 0.5")};

  REQUIRE(part::filter((p3mod <= 1_GeV_c), protons).size() == 2);
  REQUIRE(part::filter((p3mod < 1_GeV_c), protons).size() == 1);
  REQUIRE(part::filter((p3mod > 1_GeV_c), protons).size() == 1);
  REQUIRE(part::filter((p3mod >= 1_GeV_c), protons).size() == 2);

  REQUIRE(part::filter((p3mod >= 1_GeV_c) && (p3mod < 1.5_GeV_c), protons).size() ==
          1);
  REQUIRE(part::filter((p3mod > 1_GeV_c) && (p3mod < 1.5_GeV_c), protons).size() ==
          0);
  REQUIRE(part::filter((p3mod < 1.5_GeV_c) && (p3mod >= 1_GeV_c), protons).size() ==
          1);
  REQUIRE(part::filter((p3mod >= 1_GeV_c) && (p3mod < 2_GeV_c), protons).size() ==
          2);
}

TEST_CASE("theta cuts", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5 60"), BuildPart("2212 1 1 20 - 45"),
      BuildPart("2212 1 0.5 70")};

  REQUIRE_FALSE((theta < 70_deg)(protons.back()));
  REQUIRE((theta <= 70_deg)(protons.back()));
  REQUIRE((theta >= 70_deg)(protons.back()));
  REQUIRE_FALSE((theta > 70_deg)(protons.back()));

  REQUIRE_FALSE((theta > 80_deg)(protons.back()));
  REQUIRE((theta < 80_deg)(protons.back()));

  REQUIRE(((theta > 60_deg) && (theta < 75_deg))(protons.back()));

  REQUIRE_FALSE(((theta > 70_deg) && (theta < 75_deg))(protons.back()));
}

TEST_CASE("theta filter", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5 60"), BuildPart("2212 1 1 20 - 45"),
      BuildPart("2212 1 0.5 70")};

  REQUIRE(part::filter((theta <= 70_deg), protons).size() == 3);
  REQUIRE(part::filter((theta < 70_deg), protons).size() == 2);
  REQUIRE(part::filter((theta < 60_deg), protons).size() == 1);

  REQUIRE(part::filter((theta > 45_deg) && (theta < 70_deg), protons).size() ==
          1);
  REQUIRE(part::filter((theta > 60_deg) && (theta < 70_deg), protons).size() ==
          0);
  REQUIRE(part::filter((theta < 70_deg) && (theta > 60_deg), protons).size() ==
          0);
}

TEST_CASE("p3mod+theta filter", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5 60"), BuildPart("2212 1 1 20 - 45"),
      BuildPart("2212 1 0.5 70")};

  REQUIRE(part::filter((theta < 80_deg) && (theta > 65_deg) &&
                           (p3mod >= 0.75_GeV_c) && (p3mod < 2_GeV_c),
                       protons)
              .size() == 0);
  REQUIRE(part::filter((p3mod >= 0.75_GeV_c) && (p3mod < 2_GeV_c) &&
                           (theta < 80_deg) && (theta > 65_deg),
                       protons)
              .size() == 0);

  REQUIRE(part::filter((theta < 80_deg) && (theta >= 60_deg) &&
                           (p3mod >= 0.75_GeV_c) && (p3mod < 2_GeV_c),
                       protons)
              .size() == 1);
  REQUIRE(part::filter((p3mod >= 0.75_GeV_c) && (p3mod < 2_GeV_c) &&
                           (theta < 80_deg) && (theta >= 60_deg),
                       protons)
              .size() == 1);

  REQUIRE(part::filter((theta < 80_deg) && (theta > 65_deg) &&
                           (p3mod >= 0.4_GeV_c) && (p3mod < 1_GeV_c),
                       protons)
              .size() == 1);
  REQUIRE(part::filter((p3mod >= 0.4_GeV_c) && (p3mod < 1_GeV_c) &&
                           (theta < 80_deg) && (theta > 65_deg),
                       protons)
              .size() == 1);
}

TEST_CASE("sort_ascending p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  auto protons_sorted = part::sort_ascending(p3mod, protons);

  REQUIRE_THAT(protons_sorted.front()->momentum().length(),
               WithinAbs(0.5_GeV_c, 1E-8));
  REQUIRE_THAT(protons_sorted.back()->momentum().length(),
               WithinAbs(1.5_GeV_c, 1E-8));
}

TEST_CASE("highest/lowest p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE_THAT(part::highest(p3mod, protons)->momentum().length(),
               WithinAbs(1.5_GeV_c, 1E-8));
  REQUIRE_THAT(part::lowest(p3mod, protons)->momentum().length(),
               WithinAbs(0.5_GeV_c, 1E-8));
}

TEST_CASE("filter p3mod", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1.5"), BuildPart("2212 1 1"), BuildPart("2212 1 0.5")};

  REQUIRE(part::filter(p3mod > 2_GeV_c, protons).size() == 0);
  REQUIRE(part::filter(p3mod > 1.1_GeV_c, protons).size() == 1);
  REQUIRE(part::filter(p3mod > 0.25_GeV_c, protons).size() == 3);
  REQUIRE(part::filter(p3mod > 0.75_GeV_c && p3mod < 1.25_GeV_c, protons).size() ==
          1);
}

TEST_CASE("sum momentum", "[ps::part]") {

  std::vector<HepMC3::ConstGenParticlePtr> protons{
      BuildPart("2212 1 1 45 - 0"), BuildPart("2212 1 1 45 - 120")};
  std::vector<HepMC3::ConstGenParticlePtr> muons{BuildPart("13 1 1 45 - 240")};

  REQUIRE_THAT(
      part::sum(momentum, protons).x(),
      WithinAbs((std::sin(45_deg) + std::sin(45_deg) * std::cos(120_deg)) *
                    unit::GeV,
                1E-8));
  REQUIRE_THAT(
      part::sum(momentum, std::array{protons, muons}, ps::flatten).pt(),
      WithinAbs(0, 1E-8));
}
