void annotate(HepMC3::GenEvent &ev) { // NEUT specific channel annotation
  auto attr = ev.attribute<HepMC3::IntAttribute>("ProcID");
  if (!attr) {
    return;
  }

  if (std::abs(attr->value()) <= 30) {
    ev.add_attribute("IsCC", std::make_shared<HepMC3::BoolAttribute>(true));
  }

  switch (attr->value()) {
  case -1:
  case 1: {
    ev.add_attribute("Is1p1h", std::make_shared<HepMC3::BoolAttribute>(true));
    break;
  }

  case -2:
  case 2: {
    ev.add_attribute("Is2p2h", std::make_shared<HepMC3::BoolAttribute>(true));
    break;
  }

  case -11:
  case 11:
  case -12:
  case 12:
  case -13:
  case 13: {
    ev.add_attribute("IsSPP", std::make_shared<HepMC3::BoolAttribute>(true));
    ev.add_attribute("IsRES", std::make_shared<HepMC3::BoolAttribute>(true));
    break;
  }

  case -16:
  case 16: {
    ev.add_attribute("IsCOH", std::make_shared<HepMC3::BoolAttribute>(true));
    break;
  }

  case -26:
  case 26: {
    ev.add_attribute("IsDIS", std::make_shared<HepMC3::BoolAttribute>(true));
    break;
  }
  }
}

bool filt(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::pdg::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::pdg::kMuon);
  if (!nu || !mu) {
    return false;
  }

  return true;
}

double proj_elep(HepMC3::GenEvent const &ev) {
  auto mu = ps::GetOutPartHM(ev, ps::pdg::kMuon);
  return mu->momentum().e() * ps::GeV;
}

double proj_q0(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::pdg::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::pdg::kMuon);
  return ps::parts::q0(nu, mu) * ps::GeV;
}

double projev_q0(HepMC3::GenEvent const &ev) {
  return ps::event::q0(ev) * ps::GeV;
}

double proj_q3(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::pdg::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::pdg::kMuon);
  return ps::parts::q3(nu, mu) * ps::GeV;
}

double proj_Q2Lep(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::pdg::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::pdg::kMuon);
  return ps::parts::Q2Lep(nu, mu) * ps::GeV2;
}

double proj_NEUTChannel(HepMC3::GenEvent const &ev) {
  return ev.attribute<HepMC3::IntAttribute>("ProcID")->value();
}

double proj_IsCCQE(HepMC3::GenEvent const &ev) {
  if (ps::IsCC(ev) && ps::Is1p1h(ev)) {
    return 1;
  }
  return 0;
}
