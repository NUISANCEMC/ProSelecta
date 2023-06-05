#include "stdlib.h"
#include "stdio.h"

extern "C" {

bool analysis(std::string name){

  // Parse YAML Config File
  

  


  return false;
}






bool filt(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  if (!nu || !mu) {
    return false;
  }

  return true;
}

double proj_q0(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::q0(nu, mu) * ps::GeV;
}

double proj_q3(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::q3(nu, mu) * ps::GeV;
}

double proj_Q2Lep(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::Q2Lep(nu, mu) * ps::GeV2;
}

}