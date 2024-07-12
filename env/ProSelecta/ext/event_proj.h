#pragma once

#include "ProSelecta/env.h"

#include "HepMC3/GenVertex.h"

namespace ps::ext {
double enu_GeV(HepMC3::GenEvent const &ev) {
  return ps::event::beam_part(ev, ps::pdg::kNeutralLeptons)->momentum().e() /
         ps::unit::GeV;
}

double pmu_GeV(HepMC3::GenEvent const &ev) {
  if (!ps::event::has_out_part(ev, ps::pdg::kMuon)) {
    return ps::kMissingDatum<double>;
  }

  return ps::event::hm_out_part(ev, ps::pdg::kMuon)->momentum().p3mod() /
         ps::unit::GeV_c;
}

double thetamu_deg(HepMC3::GenEvent const &ev) {
  if (!ps::event::has_out_part(ev, ps::pdg::kMuon)) {
    return ps::kMissingDatum<double>;
  }

  return ps::event::hm_out_part(ev, ps::pdg::kMuon)->momentum().theta() /
         ps::unit::deg;
}

std::array<HepMC3::ConstGenParticlePtr, 2>
GetNuFSLep(HepMC3::GenEvent const &ev) {
  auto nu = ps::event::beam_part(ev, ps::pdg::kNeutralLeptons);

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

double Q2lep_GeV2(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return ps::kMissingDatum<double>;
  }

  return -(nu->momentum() - fslep->momentum()).interval() / ps::unit::GeV2;
}

double q0lep_GeV(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return ps::kMissingDatum<double>;
  }

  return (nu->momentum().e() - fslep->momentum().e()) / ps::unit::GeV;
}

double q3lep_GeV(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!fslep) {
    return ps::kMissingDatum<double>;
  }

  return (nu->momentum() - fslep->momentum()).p3mod() / ps::unit::GeV_c;
}

double hm_pprot_GeV(HepMC3::GenEvent const &ev) {
  if (!ps::event::has_out_part(ev, ps::pdg::kProton)) {
    return ps::kMissingDatum<double>;
  }

  return ps::event::hm_out_part(ev, ps::pdg::kProton)->momentum().p3mod() /
         ps::unit::GeV_c;
}

double hm_thetaprot_deg(HepMC3::GenEvent const &ev) {
  if (!ps::event::has_out_part(ev, ps::pdg::kProton)) {
    return ps::kMissingDatum<double>;
  }

  return ps::event::hm_out_part(ev, ps::pdg::kProton)->momentum().theta() /
         ps::unit::deg;
}
} // namespace ps::ext