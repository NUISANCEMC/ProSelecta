#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

using namespace ps;

TEST_CASE("readme-1", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.7", "2212 1 0.15"}},
                        {2, 1});

  if (!ps::event::has_beam_part(evt, ps::pdg::kNeutralLeptons)) {
    REQUIRE(false);
  }
  HepMC3::ConstGenParticlePtr nu =
      ps::event::beam_part(evt, ps::pdg::kNeutralLeptons);

  REQUIRE_THAT(nu->momentum().e(), WithinAbs(1 * ps::unit::GeV, 1E-8));
}

TEST_CASE("readme-2", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.7", "2212 1 0.15"}},
                        {2, 1});

  if (!ps::event::has_beam_part(evt, 14) || !ps::event::has_out_part(evt, 13)) {
    REQUIRE(false);
  }
}

TEST_CASE("readme-3", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.7", "2212 1 0.15", "211 1 0.2"}},
                        {2, 1});

  if (!ps::event::out_part_topology_matches(evt, ps::pids(13, 2212, 211),
                                            {1, 1, 1})) {
    REQUIRE(false);
  }
}

TEST_CASE("readme-4", "[ps::readme]") {

  auto evt =
      BuildEvent({{"14 4 1", "1000060120 11 0"},
                  {"14 3 1", "1000060110 1 0", "2112 21 0"},
                  {"13 1 0.7", "2212 1 0.2", "2212 1 0.15", "2212 1 0.1"}},
                 {2, 1});

  std::vector<HepMC3::ConstGenParticlePtr> protons =
      ps::event::all_out_part(evt, 2212);
  auto protons_sorted = ps::part::sort_ascending(ps::p3mod, protons);
  REQUIRE_THAT(protons_sorted[0]->momentum().p3mod(),
               WithinAbs(0.1 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(protons_sorted[1]->momentum().p3mod(),
               WithinAbs(0.15 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(protons_sorted[2]->momentum().p3mod(),
               WithinAbs(0.2 * ps::unit::GeV, 1E-8));
}

TEST_CASE("readme-5", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.7", "2212 1 0.15", "2212 1 0.2", "-211 1 0.3",
                          "-211 1 0.3", "-211 1 0.15"}},
                        {2, 1});

  auto [hmproton, hmpim] = ps::part::highest(
      ps::p3mod, ps::event::all_out_part(evt, ps::pids(2212, -211)));
  REQUIRE_THAT(hmproton->momentum().p3mod(),
               WithinAbs(0.2 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(hmpim->momentum().p3mod(), WithinAbs(0.3 * ps::unit::GeV, 1E-8));
}

TEST_CASE("readme-6", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.5 90 - 0", "2212 1 0.5 90 - 180"}},
                        {2, 1});

  auto sum_pt =
      ps::part::sum(ps::momentum, ps::event::all_out_part(
                                      evt, ps::pids(13, 2212), ps::squeeze))
          .pt() /
      ps::unit::GeV_c;
  REQUIRE_THAT(sum_pt, WithinAbs(0 * ps::unit::GeV, 1E-8));
}

TEST_CASE("readme-7", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 0.7", "2212 1 0.15", "2212 1 0.049",
                          "2212 1 2.01", "2212 1 0.75"}},
                        {2, 1});

  auto p3mod_cut =
      (ps::p3mod > 0.05 * ps::unit::GeV) && (ps::p3mod < 2 * ps::unit::GeV);
  auto passing_protons =
      ps::part::filter(p3mod_cut, ps::event::all_out_part(evt, 2212));
  REQUIRE(passing_protons.size() == 2);
}

TEST_CASE("readme-8", "[ps::readme]") {

  auto evt = BuildEvent({{"14 4 1", "1000060120 11 0"},
                         {"14 3 1", "1000060110 1 0", "2112 21 0"},
                         {"13 1 1 ", "2212 1 0.15"}},
                        {2, 1});

  auto invmass_protons_and_pions =
      ps::part::sum(ps::momentum,
                    ps::part::filter(
                        ps::p3mod > 250 * ps::unit::MeV,
                        ps::event::all_out_part(
                            evt, ps::pids(2212, 211, -211, 111), ps::squeeze)))
          .m();
}

