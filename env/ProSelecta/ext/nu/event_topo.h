#pragma once

#include "ProSelecta/env.h"

#include "ProSelecta/ext/nu/event_proj.h"

#include "HepMC3/GenVertex.h"

namespace ps::ext::nu {

using namespace ps;

int isCC(HepMC3::GenEvent const &ev) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!nu || !fslep || (nu->pid() == fslep->pid())) {
    return false;
  }
  return true;
}

int is0Pi(HepMC3::GenEvent const &ev, bool CCOrNC) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!nu || !fslep) {
    return false;
  }

  if (CCOrNC) {
    if (nu->pid() == fslep->pid()) {
      return false;
    }
  } else { // enforce NC
    if (nu->pid() != fslep->pid()) {
      return false;
    }
  }

  if (!event::has_exact_out_part(ev, fslep->pid(), 1)) {
    return false;
  }

  return event::num_out_part_except(
             ev, pids(fslep->pid(), pdg::kProton, pdg::kNeutron)) == 0;
}

int isCC0Pi(HepMC3::GenEvent const &ev) { return is0Pi(ev, true); }
int isNC0Pi(HepMC3::GenEvent const &ev) { return is0Pi(ev, false); }

int is1Pi(HepMC3::GenEvent const &ev, bool CCOrNC) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!nu || !fslep) {
    return false;
  }

  if (CCOrNC) {
    if (nu->pid() == fslep->pid()) {
      return false;
    }
  } else { // enforce NC
    if (nu->pid() != fslep->pid()) {
      return false;
    }
  }

  if (event::num_out_part(ev, pids(pdg::kPiPlus, pdg::kPiZero, pdg::kPiMinus),
                          ps::flatten) != 1) {
    return false;
  }

  return event::num_out_part_except(ev, pids(fslep->pid(), pdg::kProton,
                                             pdg::kNeutron, pdg::kPiPlus,
                                             pdg::kPiZero, pdg::kPiMinus)) == 0;
}

int isCC1Pi(HepMC3::GenEvent const &ev) { return is1Pi(ev, true); }
int isNC1Pi(HepMC3::GenEvent const &ev) { return is1Pi(ev, false); }

int isMultiPi(HepMC3::GenEvent const &ev, bool CCOrNC) {
  auto const &[nu, fslep] = GetNuFSLep(ev);

  if (!nu || !fslep) {
    return false;
  }

  if (CCOrNC) {
    if (nu->pid() == fslep->pid()) {
      return false;
    }
  } else { // enforce NC
    if (nu->pid() != fslep->pid()) {
      return false;
    }
  }

  if (event::num_out_part(ev, pids(pdg::kPiPlus, pdg::kPiZero, pdg::kPiMinus),
                          ps::flatten) < 2) {
    return false;
  }

  return event::num_out_part_except(ev, pids(fslep->pid(), pdg::kProton,
                                             pdg::kNeutron, pdg::kPiPlus,
                                             pdg::kPiZero, pdg::kPiMinus)) == 0;
}

int isCCMultiPi(HepMC3::GenEvent const &ev) { return isMultiPi(ev, true); }
int isNCMultiPi(HepMC3::GenEvent const &ev) { return isMultiPi(ev, false); }

int final_state_topology(HepMC3::GenEvent const &ev) {
  if (isCC0Pi(ev)) {
    return 0;
  } else if (isNC0Pi(ev)) {
    return 1;
  } else if (isCC1Pi(ev)) {
    return 2;
  } else if (isNC1Pi(ev)) {
    return 3;
  } else if (isCCMultiPi(ev)) {
    return 4;
  } else if (isNCMultiPi(ev)) {
    return 5;
  }
  return -1;
}

} // namespace ps::ext::nu