#pragma once

#include "detail/Selectors.h"

namespace ps {
namespace sel {

// Beam(event, PID) -> particle
HepMC3::ConstGenParticlePtr Beam(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kBeam,
                                          ProSelecta_detail::kFirst>(ev, PID);
}

// BeamAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr BeamAny(HepMC3::GenEvent const &ev,
                                    std::vector<int> const &PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kBeam,
                                     ProSelecta_detail::kFirst>(ev, PIDs);
}

// Beams(event, PID) -> list<particle>
std::vector<HepMC3::ConstGenParticlePtr> Beams(HepMC3::GenEvent const &ev,
                                               int PID) {
  return ProSelecta_detail::particles_1pdg<ProSelecta_detail::kBeam,
                                           ProSelecta_detail::kFromPDGList>(
      ev, PID);
}

// BeamsAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
BeamsAny(HepMC3::GenEvent const &ev, std::vector<int> const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam,
                                      ProSelecta_detail::kFromPDGList>(ev,
                                                                       PIDs);
}

// Tar(event) -> particle
HepMC3::ConstGenParticlePtr Target(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::particle_any<ProSelecta_detail::kTarget,
                                         ProSelecta_detail::kFirst>(ev);
}

// OutPartFirst(event, PID) -> particle
HepMC3::ConstGenParticlePtr OutPartFirst(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kUndecayedPhysical,
                                          ProSelecta_detail::kFirst>(ev, PID);
}
// OutPartFirstAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr OutPartFirstAny(HepMC3::GenEvent const &ev,
                                            std::vector<int> const &PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kUndecayedPhysical,
                                     ProSelecta_detail::kFirst>(ev, PIDs);
}

// OutPartHM(event, PID) -> particle
HepMC3::ConstGenParticlePtr OutPartHM(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kUndecayedPhysical,
                                          ProSelecta_detail::kHighestMomentum>(
      ev, PID);
}

// OutPartHMAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr OutPartHMAny(HepMC3::GenEvent const &ev,
                                         std::vector<int> const &PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kUndecayedPhysical,
                                     ProSelecta_detail::kHighestMomentum>(ev,
                                                                          PIDs);
}

// OutParts(event, PID) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr> OutParts(HepMC3::GenEvent const &ev,
                                                  int PID) {
  return ProSelecta_detail::particles_1pdg<
      ProSelecta_detail::kUndecayedPhysical, ProSelecta_detail::kFromPDGList>(
      ev, PID);
}
// OutPartsAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
OutPartsAny(HepMC3::GenEvent const &ev, std::vector<int> const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kFromPDGList>(ev,
                                                                       PIDs);
}

// OutPartsExcept(event, PID) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
OutPartsExcept(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particles_1pdg<
      ProSelecta_detail::kUndecayedPhysical,
      ProSelecta_detail::kNotFromPDGList>(ev, PID);
}
// OutPartsExceptAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
OutPartsExceptAny(HepMC3::GenEvent const &ev, std::vector<int> const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs);
}

} // namespace sel
} // namespace ps