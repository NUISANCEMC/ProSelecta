#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

TEST_CASE("BeamPart", "[ps::event]") {

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE(ps::event::HasBeamPart(evt1));
  REQUIRE(ps::event::HasBeamPart(evt1, ps::pdg::kNuMu));
  REQUIRE(!ps::event::HasBeamPart(evt1, ps::pdg::kNuE));
  REQUIRE(!ps::event::HasBeamPart(evt1, ps::pdg::kANuMu));

  REQUIRE(ps::event::BeamPart(evt1, ps::pdg::kNuMu)->pid() == ps::pdg::kNuMu);
  REQUIRE_THAT(ps::event::BeamPart(evt1, ps::pdg::kNuMu)->momentum().length(),
               WithinAbs(1, 1E-8));
}

TEST_CASE("TargetPart", "[ps::event]") {

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE(ps::event::HasTargetPart(evt1));
  REQUIRE(ps::event::HasTargetPart(evt1, 1000060120));
  REQUIRE(!ps::event::HasTargetPart(evt1, 1000070140));
  REQUIRE(ps::event::TargetPart(evt1)->pid() == 1000060120);
  REQUIRE_THAT(ps::event::TargetPart(evt1)->momentum().m(),
               WithinAbs(12, 1E-8));
}

TEST_CASE("HasOutPart", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::event::HasOutPart(evt1, 2212));
  REQUIRE(ps::event::HasOutPart(evt1, 13));
  REQUIRE(ps::event::HasOutPart(evt1, -13));
  REQUIRE(!ps::event::HasOutPart(evt1, 14));

  REQUIRE(ps::event::HasOutPart(evt1, {2212, 13, -13}));
  REQUIRE(!ps::event::HasOutPart(evt1, {2212, 13, -13, 14}));
}

TEST_CASE("HasAtLeastOutPart", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::event::HasAtLeastOutPart(evt1, 2212, 1));
  REQUIRE(!ps::event::HasAtLeastOutPart(evt1, 2212, 2));
  REQUIRE(ps::event::HasAtLeastOutPart(evt1, 13, 1));
  REQUIRE(ps::event::HasAtLeastOutPart(evt1, 13, 2));
  REQUIRE(!ps::event::HasAtLeastOutPart(evt1, 13, 3));

  REQUIRE(ps::event::HasAtLeastOutPart(evt1, {2212, 13, -13}, {1, 2, 1}));
  REQUIRE(!ps::event::HasAtLeastOutPart(evt1, {2212, 13, -13}, {1, 2, 2}));
}

TEST_CASE("NumOutPart", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::event::NumOutPart(evt1, 2212) == 1);
  REQUIRE(ps::event::NumOutPart(evt1, 13) == 2);
  REQUIRE(ps::event::NumOutPart(evt1, 14) == 0);

  auto const &[nprot, nmu, nmubar, nnumu] =
      ps::event::NumOutPart(evt1, 2212, 13, -13, 14);
  REQUIRE(nprot == 1);
  REQUIRE(nmu == 2);
  REQUIRE(nmubar == 1);
  REQUIRE(nnumu == 0);
}

TEST_CASE("NumOutPartExcept", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::event::NumOutPartExcept(evt1, 2212) == 3);
  REQUIRE(ps::event::NumOutPartExcept(evt1, 13) == 2);
  REQUIRE(ps::event::NumOutPartExcept(evt1, 14) == 4);
  REQUIRE(ps::event::NumOutPartExcept(evt1, 2212, 13, -13) == 0);
}

TEST_CASE("AllOutPart", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  auto protons = ps::event::AllOutPart(evt1, 2212);

  REQUIRE(protons.size() == 1);
  REQUIRE_THAT(protons.front()->momentum().length(), WithinAbs(0.15, 1E-8));
  REQUIRE_THAT(protons.front()->momentum().m(), WithinAbs(0.938, 1E-8));

  auto const &[muons, antimuons] = ps::event::AllOutPart(evt1, 13, -13);

  REQUIRE(muons.size() == 2);
  REQUIRE(antimuons.size() == 1);
  REQUIRE_THAT(antimuons.front()->momentum().length(), WithinAbs(1.3, 1E-8));
}

TEST_CASE("AllOutPartExcept", "[ps::event]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  auto not_protons = ps::event::AllOutPartExcept(evt1, 2212);
  REQUIRE(not_protons.size() == 3);

  auto not_protons_or_muons = ps::event::AllOutPartExcept(evt1, 2212, 13);
  REQUIRE(not_protons_or_muons.size() == 1);
}
