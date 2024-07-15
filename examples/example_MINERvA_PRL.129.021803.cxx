int MINERvA_PRL129_021803_SignalDefinition(HepMC3::GenEvent const &ev) {

  using namespace ps;
  using namespace ps::event;
  using namespace ps::part;
  using namespace ps::pdg;
  using namespace ps::unit;

  if (!has_beam_part(ev, kNuMu) || !has_exact_out_part(ev, kMuon, 1)) {
    return false;
  }

  auto nu = beam_part(ev, kNuMu);
  auto mu = hm_out_part(ev, kMuon);
  auto pmu = mu->momentum();

  // check muon angle
  if (pmu.theta() >= 20.0 * deg) {
    return false;
  }

  // check p||
  if (pmu.z() < 1.5 * GeV) {
    return false;
  }

  // Check no gammas above 10 MeV
  auto gammas = all_out_part(ev, ps::pdg::kGamma);
  for (auto &g : gammas) {
    if (g->momentum().e() >= 10 * MeV) {
      return false;
    }
  }

  auto other_parts =
      all_out_part_except(ev, pids(kNuMu, kMuon, kGamma, kProton, kNeutron));

  return (other_parts.size() == 0);
}

double MINERvA_PRL129_021803_Project_MuonE(HepMC3::GenEvent const &ev) {
  using namespace ps;
  using namespace ps::event;
  using namespace ps::pdg;
  using namespace ps::unit;

  if (!has_out_part(ev, kMuon)) {
    return kMissingDatum<double>;
  }

  return hm_out_part(ev, kMuon)->momentum().e() / GeV;
}

double MINERvA_PRL129_021803_Project_SumTp(HepMC3::GenEvent const &ev) {
  using namespace ps::event;
  using namespace ps::pdg;
  using namespace ps::unit;

  double SumTP = 0;

  for (auto const &prot : all_out_part(ev, kProton)) {
    auto const &prot_4mom = prot->momentum();
    SumTP += (prot_4mom.e() - prot_4mom.m());
  }

  return SumTP / GeV;
}

double MINERvA_PRL129_021803_Project_q0QE(HepMC3::GenEvent const &ev) {
  using namespace ps::event;
  using namespace ps::part;
  using namespace ps::pdg;
  using namespace ps::vect;
  using namespace ps::unit;

  if (!has_beam_part(ev, kNuMu) || !has_out_part(ev, kMuon)) {
    return false;
  }

  auto nu = beam_part(ev, kNuMu);
  auto mu = hm_out_part(ev, kMuon);

  auto pnu = nu->momentum();
  auto pmu = mu->momentum();

  static double const m_mu = 105.66 * MeV;
  static double const m_n = 939.565 * MeV;
  static double const m_p = 938.272 * MeV;
  static double const Eb = 34 * MeV;

  static double const numer_const =
      std::pow(m_p, 2) - std::pow(m_n - Eb, 2) - std::pow(m_mu, 2);
  static double const denom_const = 2.0 * (m_n - Eb);

  double E_mu = pmu.e();
  double p_mu = pmu.p3mod();
  double cos_mu = std::cos(angle(pnu, pmu));
  double E_mu_less_p_mu_cos_mu = (E_mu - p_mu * cos_mu);

  double numer = numer_const + 2.0 * E_mu_less_p_mu_cos_mu * E_mu;
  double denom = denom_const - E_mu_less_p_mu_cos_mu;

  return (numer / denom) / GeV;
}