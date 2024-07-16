#pragma once

#include "ProSelecta/env.h"

#include "HepMC3/GenVertex.h"

namespace ps::ext::nu {

using namespace ps;

double enu_GeV(HepMC3::GenEvent const &ev) {
  return event::beam_part(ev, pdg::kNeutralLeptons)->momentum().e() / unit::GeV;
}

std::array<HepMC3::ConstGenParticlePtr, 2>
GetNuFSLep(HepMC3::GenEvent const &ev) {
  auto nu = event::beam_part(ev, pdg::kNeutralLeptons);

  int fslep_ccpid = nu->pid() > 0 ? nu->pid() - 1 : nu->pid() + 1;

  HepMC3::ConstGenParticlePtr hmfscclep = nullptr;
  size_t nfsallccleps = 0;
  HepMC3::ConstGenParticlePtr hmfsnclep = nullptr;
  size_t nfsallncleps = 0;

  for (auto const &p : nu->end_vertex()->particles_out()) {
    if (p->pid() == fslep_ccpid) {
      hmfscclep =
          !hmfscclep || hmfscclep->momentum().p3mod() < p->momentum().p3mod()
              ? p
              : hmfscclep;
      nfsallccleps++;
    } else if (p->pid() == nu->pid()) {
      hmfsnclep =
          !hmfsnclep || hmfsnclep->momentum().p3mod() < p->momentum().p3mod()
              ? p
              : hmfsnclep;
      nfsallncleps++;
    } else if (p->pid() == -fslep_ccpid) {
      nfsallccleps++;
    } else if (p->pid() == -nu->pid()) {
      nfsallncleps++;
    }
  }

  if (!hmfscclep && !hmfsnclep) {
    return {nu, nullptr};
  } else if (hmfscclep && !hmfsnclep) {
    return {nu, hmfscclep};
  } else if (!hmfscclep && hmfsnclep) {
    return {nu, hmfsnclep};
  }

  if (nfsallccleps % 2) {
    return {nu, hmfscclep};
  } else if (nfsallncleps % 2) {
    return {nu, hmfsnclep};
  } else {
    return {nu, nullptr};
  }
}

double plep_GeV(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return kMissingDatum<double>;
  }

  return fslep->momentum().p3mod() / unit::GeV_c;
}

double thetalep_deg(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return kMissingDatum<double>;
  }

  return fslep->momentum().theta() / unit::deg;
}

double Q2lep_GeV2(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return kMissingDatum<double>;
  }

  return -(nu->momentum() - fslep->momentum()).interval() / unit::GeV2;
}

double q0lep_GeV(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return kMissingDatum<double>;
  }

  return (nu->momentum().e() - fslep->momentum().e()) / unit::GeV;
}

double q3lep_GeV(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return kMissingDatum<double>;
  }

  return (nu->momentum() - fslep->momentum()).p3mod() / unit::GeV_c;
}

double hm_pprot_GeV(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kProton)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kProton)->momentum().p3mod() / unit::GeV_c;
}

double hm_thetaprot_deg(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kProton)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kProton)->momentum().theta() / unit::deg;
}

double hm_ppip_GeV(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiPlus)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiPlus)->momentum().p3mod() / unit::GeV_c;
}

double hm_thetapip_deg(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiPlus)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiPlus)->momentum().theta() / unit::deg;
}

double hm_ppim_GeV(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiMinus)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiMinus)->momentum().p3mod() /
         unit::GeV_c;
}

double hm_thetapim_deg(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiMinus)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiMinus)->momentum().theta() / unit::deg;
}

double hm_ppi0_GeV(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiZero)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiZero)->momentum().p3mod() / unit::GeV_c;
}

double hm_thetapi0_deg(HepMC3::GenEvent const &ev) {
  if (!event::has_out_part(ev, pdg::kPiZero)) {
    return kMissingDatum<double>;
  }

  return event::hm_out_part(ev, pdg::kPiZero)->momentum().theta() / unit::deg;
}

} // namespace ps::ext::nu