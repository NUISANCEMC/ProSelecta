#pragma once

#include "detail/Selectors.h"

#include <stdexcept>

namespace ps {
namespace sel {

struct EmptyPIDList : public std::exception {};
struct NoMatchingParts : public std::exception {};
struct MoreThanOneBeamPart : public std::exception {};
struct MoreThanOneTargetPart : public std::exception {};

//[Has, HasAtLeast, Num, NumExcept, All, AllExcept,
// HM][Out,Beam,Target]Part

template <size_t N>
bool HasOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  if (!PIDs.size()) {
    throw EmptyPIDList();
  }
  return has_particles<ProSelecta_detail::kUndecayedPhysical>(ev, PIDs);
}

bool HasOutPart(HepMC3::GenEvent const &ev, int PID) {
  return has_particles<ProSelecta_detail::kUndecayedPhysical>(ev, {PID});
}

template <size_t N>
bool HasAtLeastOutPart(HepMC3::GenEvent const &ev,
                       std::array<int, N> const &PIDs,
                       std::array<int, N> const &counts) {
  if (!PIDs.size()) {
    throw EmptyPIDList();
  }
  return has_particles_atleast<ProSelecta_detail::kUndecayedPhysical>(ev, PIDs,
                                                                      counts);
}

bool HasAtLeastOutPart(HepMC3::GenEvent const &ev, int PID, int count) {
  return has_particles_atleast<ProSelecta_detail::kUndecayedPhysical>(ev, {PID},
                                                                      {count});
}

template <size_t N>
std::array<int, N> NumOutPart(HepMC3::GenEvent const &ev,
                              std::array<int, N> const &PIDs) {
  if (!PIDs.size()) {
    throw EmptyPIDList();
  }
  std::array<int, N> outs;
  for (size_t i = 0; i < N; ++i) {
    outs[i] =
        particles<ProSelecta_detail::kUndecayedPhysical>(ev, {PIDs[i]}).size();
  }
  return outs;
}

int NumOutPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return particles<ProSelecta_detail::kUndecayedPhysical>(ev, {PID}).size();
  }

  return particles<ProSelecta_detail::kUndecayedPhysical>(ev, {}).size();
}

template <size_t N>
int NumExceptOutPart(HepMC3::GenEvent const &ev,
                     std::array<int, N> const &PIDs) {
  if (!PIDs.size()) {
    throw EmptyPIDList();
  }

  return particles<ProSelecta_detail::kUndecayedPhysical,
                   ProSelecta_detail::kNotFromPDGList>(ev, PIDs)
      .size();
}

int NumExceptOutPart(HepMC3::GenEvent const &ev, int PID) {
  return particles<ProSelecta_detail::kUndecayedPhysical,
                   ProSelecta_detail::kNotFromPDGList>(ev, {PID})
      .size();
}

template <size_t N>
std::array<std::vector<HepMC3::ConstGenParticlePtr>, N>
AllOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  if (!PIDs.size()) {
    throw EmptyPIDList();
  }
  std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> outs;
  for (size_t i = 0; i < N; ++i) {
    outs[i] = particles<ProSelecta_detail::kUndecayedPhysical>(ev, {PIDs[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr> AllOutPart(HepMC3::GenEvent const &ev,
                                                    int PID) {
  return particles<ProSelecta_detail::kUndecayedPhysical>(ev, {PID});
}

std::vector<HepMC3::ConstGenParticlePtr>
AllOutPartExcept(HepMC3::GenEvent const &ev, int PID) {
  return particles<ProSelecta_detail::kUndecayedPhysical,
                   ProSelecta_detail::kNotFromPDGList>(ev, {PID});
}

template <size_t N>
std::array<HepMC3::ConstGenParticlePtr, N>
HMOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  std::array<HepMC3::ConstGenParticlePtr, N> outs;
  for (size_t i = 0; i < N; ++i) {
    outs[i] = hmparticle<ProSelecta_detail::kUndecayedPhysical>(ev, {PIDs[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr> HMOutPart(HepMC3::GenEvent const &ev,
                                                   int PID) {
  return hmparticle<ProSelecta_detail::kUndecayedPhysical>(ev, {PID});
}

bool HasBeamPart(HepMC3::GenEvent const &ev, int PID) {
  return has_particles<ProSelecta_detail::kBeam>(ev, {PID});
}

bool HasTargetPart(HepMC3::GenEvent const &ev, int PID) {
  return has_particles<ProSelecta_detail::kTarget>(ev, {PID});
}

bool BeamPart(HepMC3::GenEvent const &ev, int PID) {
  auto parts = particles<ProSelecta_detail::kBeam>(ev, {PID});
  if (!parts.size()) {
    throw NoMatchingParts();
  }
  if (parts.size() > 1) {
    throw MoreThanOneBeamPart();
  }
  return parts.front();
}

bool TargetPart(HepMC3::GenEvent const &ev, int PID) {
  auto parts = particles<ProSelecta_detail::kTarget>(ev, {PID});
  if (!parts.size()) {
    throw NoMatchingParts();
  }
  if (parts.size() > 1) {
    throw MoreThanOneTargetPart();
  }
  return parts.front();
}

std::vector<HepMC3::ConstGenParticlePtr>
OutNuclearParts(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::nuclear_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev);
}

} // namespace sel
} // namespace ps
