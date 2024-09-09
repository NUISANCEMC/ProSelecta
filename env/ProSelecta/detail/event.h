#pragma once

#include "ProSelecta/pdg.h"

#include "ProSelecta/detail/constants.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"

#include <string>
#include <vector>

namespace ps::detail {

template <int status, typename Collection, bool select_from_pdg_list = true>
inline std::vector<HepMC3::ConstGenParticlePtr>
particles(HepMC3::GenEvent const &evt, Collection const &pdgs) {

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

    if constexpr (std::is_same_v<Collection, std::array<int, 0>>) {
      selected_parts.push_back(part);
    } else if (std::is_same_v<Collection, std::vector<int>> &&
               (pdgs.size() == 0)) {
      selected_parts.push_back(part);
    } else {
      bool in_pdg_list =
          std::find(pdgs.begin(), pdgs.end(), part->pid()) != pdgs.end();
      if (in_pdg_list == select_from_pdg_list) {
        selected_parts.push_back(part);
      }
    }
  }
  return selected_parts;
}

template <int status, typename Collection>
inline bool has_particles(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  bool hasall = true;

  for (auto id : PIDs) {
    hasall = hasall && bool(particles<status>(ev, std::array{id}).size());
  }

  return hasall;
}

template <int status, typename Collection>
inline bool has_particles_exact(HepMC3::GenEvent const &ev,
                                Collection const &PIDs,
                                Collection const &counts) {
  bool hasall = true;

  for (size_t i = 0; i < PIDs.size(); ++i) {
    hasall =
        hasall && bool(typename Collection::value_type(particles<status>(ev,
                                                                std::array{
                                                                    PIDs[i],
                                                                })
                                                  .size()) == counts[i]);
  }

  return hasall;
}

template <int status, typename Collection>
inline bool has_particles_atleast(HepMC3::GenEvent const &ev,
                                  Collection const &PIDs,
                                  Collection const &counts) {
  bool hasall = true;

  for (size_t i = 0; i < PIDs.size(); ++i) {
    hasall =
        hasall && bool(typename Collection::value_type(particles<status>(ev,
                                                                std::array{
                                                                    PIDs[i],
                                                                })
                                                  .size()) >= counts[i]);
  }

  return hasall;
}

template <int status>
inline std::vector<HepMC3::ConstGenParticlePtr>
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

} // namespace ps::detail