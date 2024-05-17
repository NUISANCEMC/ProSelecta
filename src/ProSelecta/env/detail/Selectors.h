#pragma once

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"

#include "NuHepMC/Constants.hxx"
#include "NuHepMC/EventUtils.hxx"

#include <vector>

namespace ProSelecta_detail {

constexpr int kUndecayedPhysical = NuHepMC::ParticleStatus::UndecayedPhysical;
constexpr int kBeam = NuHepMC::ParticleStatus::IncomingBeam;
constexpr int kTarget = NuHepMC::ParticleStatus::Target;

constexpr bool kFromPDGList = true;
constexpr bool kNotFromPDGList = false;

template <int status>
std::vector<HepMC3::ConstGenParticlePtr>
particles_any(HepMC3::GenEvent const &evt) {

  std::vector<HepMC3::ConstGenParticlePtr> selected_parts = {};

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }
    if constexpr (status == kUndecayedPhysical) {
      if (part->pid() >= ps::pdg::kNuclearPDGBoundary) {
        continue;
      }
    }

    selected_parts.push_back(part);
  }
  return selected_parts;
}

template <int status, bool select_from_pdg_list = true>
std::vector<HepMC3::ConstGenParticlePtr> particles(HepMC3::GenEvent const &evt,
                                                   std::vector<int> pdgs) {

  std::vector<HepMC3::ConstGenParticlePtr> selected_parts = {};

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }
    if constexpr (status == kUndecayedPhysical) {
      if (part->pid() >= ps::pdg::kNuclearPDGBoundary) {
        continue;
      }
    }

    if (!pdgs.size() || ((std::find(pdgs.begin(), pdgs.end(), part->pid()) !=
                          pdgs.end()) == select_from_pdg_list)) {
      selected_parts.push_back(part);
    }
  }
  return selected_parts;
}

template <int status>
HepMC3::ConstGenParticlePtr particle(HepMC3::GenEvent const &evt,
                                     std::vector<int> pdgs) {

  HepMC3::ConstGenParticlePtr hmpart = nullptr;
  double hmmom2 = 0;

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }
    if constexpr (status == kUndecayedPhysical) {
      if (part->pid() >= ps::pdg::kNuclearPDGBoundary) {
        continue;
      }
    }

    if (!pdgs.size() ||
        (std::find(pdgs.begin(), pdgs.end(), part->pid()) != pdgs.end())) {

      double part_mom2 = part->momentum().length2();
      if (part_mom2 > hmmom2) {
        hmpart = part;
        hmmom2 = part_mom2;
      }
    }
  }

  return hmpart;
}

// Provides a convenience overload for selecting on a single pdg
template <int status, bool select_from_pdg_list = true>
std::vector<HepMC3::ConstGenParticlePtr>
particles_1pdg(HepMC3::GenEvent const &evt, int pdg) {
  return particles<status, select_from_pdg_list>(evt, {
                                                          pdg,
                                                      });
}

// Provides a convenience overload for selecting on a single pdg
template <int status>
HepMC3::ConstGenParticlePtr particle_1pdg(HepMC3::GenEvent const &evt,
                                          int pdg) {
  return particle<status>(evt, {
                                   pdg,
                               });
}

template <int status>
std::vector<HepMC3::ConstGenParticlePtr>
nuclear_particles(HepMC3::GenEvent const &evt) {

  std::vector<HepMC3::ConstGenParticlePtr> selected_parts = {};

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }
    if (part->pid() < ps::pdg::kNuclearPDGBoundary) {
      continue;
    }

    selected_parts.push_back(part);
  }
  return selected_parts;
}

} // namespace ProSelecta_detail