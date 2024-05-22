#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

using namespace ps;

TEST_CASE("beam_part", "[ps::event]") {

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE(event::has_beam_part(evt1));
  REQUIRE(event::has_beam_part(evt1, pdg::kNuMu));
  REQUIRE_FALSE(event::has_beam_part(evt1, pdg::kNuE));
  REQUIRE_FALSE(event::has_beam_part(evt1, pdg::kANuMu));
  REQUIRE(event::has_beam_part(evt1, pdg::kNeutralLeptons));

  REQUIRE(event::beam_part(evt1, pdg::kNuMu)->pid() == pdg::kNuMu);
  REQUIRE_THAT(event::beam_part(evt1, pdg::kNuMu)->momentum().length(),
               WithinAbs(1, 1E-8));

  REQUIRE(event::beam_part(evt1, pdg::kNeutralLeptons)->pid() == pdg::kNuMu);

  REQUIRE_THROWS_AS(event::beam_part(evt1, pdg::kNuE), event::NoMatchingParts);

  auto evt2 = BuildEvent({{"14 4 1", "12 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE_THROWS_AS(event::beam_part(evt2, pdg::kNeutralLeptons),
                    event::MoreThanOneBeamPart);
}

TEST_CASE("target_part", "[ps::event]") {

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE(event::has_target_part(evt1));
  REQUIRE(event::has_target_part(evt1, 1000060120));
  REQUIRE_FALSE(event::has_target_part(evt1, 1000070140));
  REQUIRE(event::target_part(evt1)->pid() == 1000060120);
  REQUIRE_THAT(event::target_part(evt1)->momentum().m(), WithinAbs(12, 1E-8));

  REQUIRE_THROWS_AS(event::target_part(evt1, 1000070140),
                    event::NoMatchingParts);
}

TEST_CASE("has_out_part", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(event::has_out_part(evt1, 2212));
  REQUIRE(event::has_out_part(evt1, 13));
  REQUIRE(event::has_out_part(evt1, -13));
  REQUIRE_FALSE(event::has_out_part(evt1, 14));

  REQUIRE(event::has_out_part(evt1, pids{2212, 13, -13}));
  REQUIRE_FALSE(event::has_out_part(evt1, pids{2212, 13, -13, 14}));
}

TEST_CASE("has_exact_out_part", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(event::has_exact_out_part(evt1, 2212, 1));
  REQUIRE(event::has_exact_out_part(evt1, 13, 2));
  REQUIRE(event::has_exact_out_part(evt1, -13, 1));
  REQUIRE(event::has_exact_out_part(evt1, 14, 0));
  REQUIRE_FALSE(event::has_exact_out_part(evt1, -13, 2));

  REQUIRE(event::has_exact_out_part(evt1, pids{2212, 13, -13}, {1, 2, 1}));
  REQUIRE(event::has_exact_out_part(evt1, pids{2212, 13, -13}, {1, 2, 1}));
  REQUIRE_FALSE(
      event::has_exact_out_part(evt1, pids{2212, 13, -13}, {1, 2, 2}));
}

TEST_CASE("out_part_topology_matches", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(
      event::out_part_topology_matches(evt1, pids{2212, 13, -13}, {1, 2, 1}));
  REQUIRE(event::out_part_topology_matches(evt1, pids{2212, 13, -13, 14},
                                           {1, 2, 1, 0}));

  REQUIRE_FALSE(event::out_part_topology_matches(evt1, pids{2212, 13}, {1, 2}));
  REQUIRE_FALSE(
      event::out_part_topology_matches(evt1, pids{2212, 13, -13}, {1, 2, 2}));
  REQUIRE_FALSE(
      event::out_part_topology_matches(evt1, pids{2212, 13, -13}, {1, 2, 2}));
}

TEST_CASE("has_at_least_out_part", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(event::has_at_least_out_part(evt1, 2212, 1));
  REQUIRE_FALSE(event::has_at_least_out_part(evt1, 2212, 2));
  REQUIRE(event::has_at_least_out_part(evt1, 13, 1));
  REQUIRE(event::has_at_least_out_part(evt1, 13, 2));
  REQUIRE_FALSE(event::has_at_least_out_part(evt1, 13, 3));

  REQUIRE(event::has_at_least_out_part(evt1, pids{2212, 13, -13}, {1, 2, 1}));
  REQUIRE(!event::has_at_least_out_part(evt1, pids{2212, 13, -13}, {1, 2, 2}));
}

TEST_CASE("num_out_part", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(event::num_out_part(evt1, 2212) == 1);
  REQUIRE(event::num_out_part(evt1, 13) == 2);
  REQUIRE(event::num_out_part(evt1, 14) == 0);

  auto const &[nprot, nmu, nmubar, nnumu] =
      event::num_out_part(evt1, pids{2212, 13, -13, 14});
  REQUIRE(nprot == 1);
  REQUIRE(nmu == 2);
  REQUIRE(nmubar == 1);
  REQUIRE(nnumu == 0);
}

TEST_CASE("num_out_part_except", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(event::num_out_part_except(evt1, 2212) == 3);
  REQUIRE(event::num_out_part_except(evt1, 13) == 2);
  REQUIRE(event::num_out_part_except(evt1, 14) == 4);
  REQUIRE(event::num_out_part_except(evt1, pids{2212, 13, -13}) == 0);
}

TEST_CASE("all_out_part", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  auto protons = event::all_out_part(evt1, 2212);

  REQUIRE(protons.size() == 1);
  REQUIRE_THAT(protons.front()->momentum().length(), WithinAbs(0.15, 1E-8));
  REQUIRE_THAT(protons.front()->momentum().m(), WithinAbs(0.938, 1E-8));

  auto const &[muons, antimuons] = event::all_out_part(evt1, pids{13, -13});

  REQUIRE(muons.size() == 2);
  REQUIRE(antimuons.size() == 1);
  REQUIRE_THAT(antimuons.front()->momentum().length(), WithinAbs(1.3, 1E-8));

  auto all_out_part = event::all_out_part(evt1);
  REQUIRE(all_out_part.size() == 4);
}

TEST_CASE("all_out_part_except", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  auto not_protons = event::all_out_part_except(evt1, 2212);
  REQUIRE(not_protons.size() == 3);

  auto not_protons_or_muons = event::all_out_part_except(evt1, pids{2212, 13});
  REQUIRE(not_protons_or_muons.size() == 1);
}
