#include "ProSelecta/env/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"

#include <cassert>

TEST_CASE("BuildMinPart", "[test-tools]") {
  auto part = BuildPart("2212 11 1");
  REQUIRE(part->pid() == 2212);
  REQUIRE(part->status() == 11);
  REQUIRE(std::fabs(part->momentum().length() - 1) < 1E-8);
  REQUIRE(std::fabs(part->momentum().m() - .938) < 1E-8);
}

TEST_CASE("BuildPartSkipMass", "[test-tools]") {
  auto part = BuildPart("2212 11 1 30 - 0");
  REQUIRE(std::fabs((part->momentum().theta() * (180.0 / M_PI)) - 30) < 1E-8);
  REQUIRE(std::fabs(part->momentum().x() - 0.5) < 1E-8);
  REQUIRE(std::fabs(part->momentum().y() - 0) < 1E-8);
  REQUIRE(std::fabs(part->momentum().z() - std::sqrt(1 - std::pow(.5, 2))) <
          1E-8);
  REQUIRE(std::fabs(part->momentum().m() - .938) < 1E-8);
  REQUIRE(std::fabs(part->momentum().phi() * (180.0 / M_PI)) < 1E-8);
}

TEST_CASE("BuildFullPart", "[test-tools]") {
  auto part = BuildPart("2212 11 1 20 .937 45");
  REQUIRE(part->pid() == 2212);
  REQUIRE(part->status() == 11);
  REQUIRE(std::fabs(part->momentum().length() - 1) < 1E-8);
  REQUIRE(std::fabs((part->momentum().theta() * (180.0 / M_PI)) - 20) < 1E-8);
  REQUIRE(std::fabs(part->momentum().m() - .937) < 1E-8);
  REQUIRE(std::fabs((part->momentum().phi() * (180.0 / M_PI)) - 45) < 1E-8);
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

TEST_CASE("BeamTarget", "[ps::sel]") {

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  REQUIRE(ps::sel::Beam(evt1, ps::pdg::kNuMu));
  REQUIRE(std::fabs(ps::sel::Beam(evt1, ps::pdg::kNuMu)->momentum().length() -
                    1) < 1E-8);
  REQUIRE(!ps::sel::Beam(evt1, ps::pdg::kANuMu));

  REQUIRE(ps::sel::BeamAny(evt1, ps::pdg::groups::kNeutralLeptons));
  REQUIRE(!ps::sel::BeamAny(evt1, ps::pdg::groups::kNeutralLeptons_amatter));

  REQUIRE(ps::sel::Beams(evt1, ps::pdg::kNuMu).size() == 1);
  REQUIRE(ps::sel::Beams(evt1, ps::pdg::kANuMu).size() == 0);

  REQUIRE(ps::sel::BeamsAny(evt1, ps::pdg::groups::kNeutralLeptons).size() ==
          1);
  REQUIRE(ps::sel::BeamsAny(evt1, ps::pdg::groups::kNeutralLeptons_amatter)
              .size() == 0);

  REQUIRE(ps::sel::Target(evt1));
  REQUIRE(ps::sel::Target(evt1)->pid() == 1000060120);
  REQUIRE(std::fabs(ps::sel::Target(evt1)->momentum().m() - 12) < 1E-8);
}

TEST_CASE("OutFirst", "[ps::sel]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(std::fabs(
              ps::sel::OutPartFirst(evt1, ps::pdg::kMuon)->momentum().length() -
              0.7) < 1E-8);
  REQUIRE(
      std::fabs(
          ps::sel::OutPartFirst(evt1, ps::pdg::kAMuon)->momentum().length() -
          1.3) < 1E-8);
  REQUIRE(
      std::fabs(
          ps::sel::OutPartFirst(evt1, ps::pdg::kProton)->momentum().length() -
          0.15) < 1E-8);
  REQUIRE(!ps::sel::OutPartFirst(evt1, ps::pdg::kNuMu));

  REQUIRE(std::fabs(
              ps::sel::OutPartFirstAny(evt1, {ps::pdg::kAMuon, ps::pdg::kMuon})
                  ->momentum()
                  .length() -
              0.7) < 1E-8);
  REQUIRE(std::fabs(
              ps::sel::OutPartFirstAny(evt1, {ps::pdg::kProton, ps::pdg::kMuon})
                  ->momentum()
                  .length() -
              0.15) < 1E-8);
  REQUIRE(!ps::sel::OutPartFirstAny(evt1, {ps::pdg::kNuMu, ps::pdg::kNeutron}));
}

TEST_CASE("OutHM", "[ps::sel]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(
      std::fabs(ps::sel::OutPartHM(evt1, ps::pdg::kMuon)->momentum().length() -
                1.2) < 1E-8);
  REQUIRE(
      std::fabs(ps::sel::OutPartHM(evt1, ps::pdg::kAMuon)->momentum().length() -
                1.3) < 1E-8);
  REQUIRE(std::fabs(
              ps::sel::OutPartHM(evt1, ps::pdg::kProton)->momentum().length() -
              0.15) < 1E-8);
  REQUIRE(!ps::sel::OutPartHM(evt1, ps::pdg::kNuMu));

  REQUIRE(
      std::fabs(ps::sel::OutPartHMAny(evt1, {ps::pdg::kAMuon, ps::pdg::kMuon})
                    ->momentum()
                    .length() -
                1.3) < 1E-8);
  REQUIRE(
      std::fabs(ps::sel::OutPartHMAny(evt1, {ps::pdg::kProton, ps::pdg::kMuon})
                    ->momentum()
                    .length() -
                1.2) < 1E-8);
  REQUIRE(!ps::sel::OutPartHMAny(evt1, {ps::pdg::kNuMu, ps::pdg::kNeutron}));
}

TEST_CASE("OutParts", "[ps::sel]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::sel::OutParts(evt1, ps::pdg::kMuon).size() == 2);
  REQUIRE(std::fabs(
              ps::sel::OutParts(evt1, ps::pdg::kMuon)[1]->momentum().length() -
              1.2) < 1E-8);
  REQUIRE(ps::sel::OutParts(evt1, ps::pdg::kAMuon).size() == 1);
  REQUIRE(ps::sel::OutParts(evt1, ps::pdg::kProton).size() == 1);
  REQUIRE(ps::sel::OutParts(evt1, ps::pdg::kNuMu).size() == 0);

  REQUIRE(
      std::fabs(ps::sel::OutPartsAny(evt1, {ps::pdg::kAMuon, ps::pdg::kMuon})[2]
                    ->momentum()
                    .length() -
                1.3) < 1E-8);
  REQUIRE(std::fabs(
              ps::sel::OutPartsAny(evt1, {ps::pdg::kProton, ps::pdg::kMuon})[0]
                  ->momentum()
                  .length() -
              0.15) < 1E-8);
  REQUIRE(
      ps::sel::OutPartsAny(evt1, {ps::pdg::kNuMu, ps::pdg::kNeutron}).size() ==
      0);
}

TEST_CASE("OutPartsExcept", "[ps::sel]") {

  auto evt1 =
      BuildEvent({{"14 4 3 0", "1000060120 11 0"},
                  {"2212 1 0.15", "13 1 0.7", "13 1 1.2", "-13 1 1.3"}});

  REQUIRE(ps::sel::OutPartsExcept(evt1, ps::pdg::kMuon).size() == 2);
  REQUIRE(std::fabs(ps::sel::OutPartsExcept(evt1, ps::pdg::kMuon)[1]
                        ->momentum()
                        .length() -
                    1.3) < 1E-8);
  REQUIRE(ps::sel::OutPartsExcept(evt1, ps::pdg::kAMuon).size() == 3);
  REQUIRE(ps::sel::OutPartsExcept(evt1, ps::pdg::kProton).size() == 3);
  REQUIRE(ps::sel::OutPartsExcept(evt1, ps::pdg::kNuMu).size() == 4);

  REQUIRE(std::fabs(ps::sel::OutPartsExceptAny(
                        evt1, {ps::pdg::kAMuon, ps::pdg::kMuon})[0]
                        ->momentum()
                        .length() -
                    0.15) < 1E-8);
  REQUIRE(std::fabs(ps::sel::OutPartsExceptAny(
                        evt1, {ps::pdg::kProton, ps::pdg::kMuon})[0]
                        ->momentum()
                        .length() -
                    1.3) < 1E-8);
  REQUIRE(ps::sel::OutPartsExceptAny(evt1, {ps::pdg::kNuMu, ps::pdg::kNeutron})
              .size() == 4);
}

TEST_CASE("HardScatterSelector", "[channel-selector]") {

  HepMC3::GenEvent evt_coh;
  evt_coh.add_attribute("IsCOH", std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(ps::IsCOH(evt_coh));
  REQUIRE(!ps::Is1p1h(evt_coh));
  REQUIRE(!ps::Is2p2h(evt_coh));
  REQUIRE(!ps::IsSPP(evt_coh));
  REQUIRE(!ps::IsRES(evt_coh));
  REQUIRE(!ps::IsDIS(evt_coh));

  HepMC3::GenEvent evt_qe;
  evt_qe.add_attribute("Is1p1h", std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(!ps::IsCOH(evt_qe));
  REQUIRE(ps::Is1p1h(evt_qe));
  REQUIRE(!ps::Is2p2h(evt_qe));
  REQUIRE(!ps::IsSPP(evt_qe));
  REQUIRE(!ps::IsRES(evt_qe));
  REQUIRE(!ps::IsDIS(evt_qe));

  HepMC3::GenEvent evt_mec;
  evt_mec.add_attribute("Is2p2h",
                        std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(!ps::IsCOH(evt_mec));
  REQUIRE(!ps::Is1p1h(evt_mec));
  REQUIRE(ps::Is2p2h(evt_mec));
  REQUIRE(!ps::IsSPP(evt_mec));
  REQUIRE(!ps::IsRES(evt_mec));
  REQUIRE(!ps::IsDIS(evt_mec));

  HepMC3::GenEvent evt_resspp;
  evt_resspp.add_attribute("IsSPP",
                           std::make_shared<HepMC3::BoolAttribute>(true));
  evt_resspp.add_attribute("IsRES",
                           std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(!ps::IsCOH(evt_resspp));
  REQUIRE(!ps::Is1p1h(evt_resspp));
  REQUIRE(!ps::Is2p2h(evt_resspp));
  REQUIRE(ps::IsSPP(evt_resspp));
  REQUIRE(ps::IsRES(evt_resspp));
  REQUIRE(!ps::IsDIS(evt_resspp));

  HepMC3::GenEvent evt_res;
  evt_res.add_attribute("IsRES", std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(!ps::IsCOH(evt_res));
  REQUIRE(!ps::Is1p1h(evt_res));
  REQUIRE(!ps::Is2p2h(evt_res));
  REQUIRE(!ps::IsSPP(evt_res));
  REQUIRE(ps::IsRES(evt_res));
  REQUIRE(!ps::IsDIS(evt_res));

  HepMC3::GenEvent evt_dis;
  evt_dis.add_attribute("IsDIS", std::make_shared<HepMC3::BoolAttribute>(true));
  REQUIRE(!ps::IsCOH(evt_dis));
  REQUIRE(!ps::Is1p1h(evt_dis));
  REQUIRE(!ps::Is2p2h(evt_dis));
  REQUIRE(!ps::IsSPP(evt_dis));
  REQUIRE(!ps::IsRES(evt_dis));
  REQUIRE(ps::IsDIS(evt_dis));
}

TEST_CASE("EventProjectors", "[ps::proj]") {

  auto evt1 = BuildEvent({{"14 4 2 0"}, {"13 1 1 30"}});

  double elep = std::sqrt(1 + std::pow(default_mass_GeV(13), 2));
  double q0 = 2 - elep;
  REQUIRE(std::fabs(ps::proj::event::q0(evt1) - q0) < 1E-8);

  HepMC3::FourVector pnu{0, 0, 2, 0};
  HepMC3::FourVector pmu{0, std::sin(30 * M_PI / 180.0),
                         std::cos(30 * M_PI / 180.0), 0};
  double q3 = (pnu - pmu).length();

  REQUIRE(std::fabs(ps::proj::event::q3(evt1) - q3) < 1E-8);

  double Q2 = q3 * q3 - q0 * q0;
  REQUIRE(std::fabs(ps::proj::event::Q2Lep(evt1) - Q2) < 1E-8);

  // Check that we grab the only allowed lepton
  auto evt2 = BuildEvent({{"14 4 2 0"},
                          {"11 1 1.5 20",
                           "-11 1 0.7 25"
                           "-13 1 1.5 45",
                           "13 1 1 30"}});
  REQUIRE(std::fabs(ps::proj::event::q0(evt2) - q0) < 1E-8);
  REQUIRE(std::fabs(ps::proj::event::q3(evt2) - q3) < 1E-8);
  REQUIRE(std::fabs(ps::proj::event::Q2Lep(evt2) - Q2) < 1E-8);

  // bad event, expected final state lepton with pid 14 or 13
  auto evt3 = BuildEvent({{"14 4 2 0"}, {"11 1 1.5 20"}});
  REQUIRE(ps::proj::event::q0(evt3) == 0xdeadbeef);
  REQUIRE(ps::proj::event::q3(evt3) == 0xdeadbeef);
  REQUIRE(ps::proj::event::Q2Lep(evt3) == 0xdeadbeef);
}
