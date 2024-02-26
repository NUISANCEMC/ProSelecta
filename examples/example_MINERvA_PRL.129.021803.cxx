int MINERvA_PRL129_021803_SignalDefinition(HepMC3::GenEvent const &ev) {
  auto nu = ps::sel::Beam(ev, ps::pdg::kNuMu);
  auto mu = ps::sel::OutPartHM(ev, ps::pdg::kMuon);
  if (!nu || !mu) {
    return false;
  }

  auto ct = ps::proj::parts::CosTheta(nu, mu);

  // check muon angle
  if ((std::acos(ct) * ps::deg) >= 20.0) {
    return false;
  }

  // check p||
  if ((ct * mu->momentum().length() * ps::GeV) < 1.5) {
    return false;
  }

  // Check no gammas above 10 MeV
  auto gammas = ps::sel::OutParts(ev, ps::pdg::kGamma);
  for (auto &g : gammas) {
    if ((g->momentum().e() * ps::MeV) >= 10) {
      return false;
    }
  }

  auto other_parts = ps::sel::OutPartsExceptAny(
      ev, {ps::pdg::kNuMu, ps::pdg::kMuon, ps::pdg::kGamma, ps::pdg::kProton,
           ps::pdg::kNeutron});

  return (other_parts.size() == 0);
}

double MINERvA_PRL129_021803_Project_MuonE(HepMC3::GenEvent const &ev) {
  return ps::sel::OutPartHM(ev, ps::pdg::kMuon)->momentum().e() * ps::GeV;
}

double MINERvA_PRL129_021803_Project_SumTp(HepMC3::GenEvent const &ev) {
  double SumTP = 0;

  for (auto const &prot : ps::sel::OutParts(ev, 2212)) {
    auto const &prot_4mom = prot->momentum();
    SumTP += (prot_4mom.e() - prot_4mom.m());
  }

  return SumTP * ps::GeV;
}

double MINERvA_PRL129_021803_Project_q0QE(HepMC3::GenEvent const &ev) {

  auto nu = ps::sel::Beam(ev, ps::pdg::kNuMu);
  auto mu = ps::sel::OutPartHM(ev, ps::pdg::kMuon);

  auto nu_4mom = nu->momentum();
  auto mu_4mom = mu->momentum();

  static double const m_mu = 105.66;
  static double const m_n = 939.565;
  static double const m_p = 938.272;
  static double const Eb = 34;

  static double const numer_const =
      std::pow(m_p, 2) - std::pow(m_n - Eb, 2) - std::pow(m_mu, 2);
  static double const denom_const = 2.0 * (m_n - Eb);

  double E_mu = mu_4mom.e();
  double p_mu = mu_4mom.p3mod();
  double cos_mu = ps::proj::parts::CosTheta(nu, mu);
  double E_mu_less_p_mu_cos_mu = (E_mu - p_mu * cos_mu);

  double numer = numer_const + 2.0 * E_mu_less_p_mu_cos_mu * E_mu;
  double denom = denom_const - E_mu_less_p_mu_cos_mu;

  return (numer / denom) * ps::GeV;
}