extern "C" {

bool filt(HepMC3::GenEvent const &ev){
  auto nu = GetBeam(ev,14);
  auto mu = GetOutPartHM(ev,13);
  if(!nu || !mu){
    return false;
  }

  return true;
}

double proj1(HepMC3::GenEvent const &ev){
  auto mu = GetOutPartHM(ev,13);
  return mu->momentum().e() * GeV;
}

double proj2(HepMC3::GenEvent const &ev){
  auto mu = GetOutPartHM(ev,13);
  return mu->momentum().pt() * GeV;
}

}