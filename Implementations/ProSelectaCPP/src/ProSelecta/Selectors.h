#pragma once

#include "ProSelecta/detail/Selectors.h"

namespace ps {

// GetBeam(event, PID) -> particle
HepMC3::ConstGenParticlePtr GetBeam(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kBeam,
                                          ProSelecta_detail::kFirst>(ev, PID);
}

// GetBeamAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr GetBeamAny(HepMC3::GenEvent const &ev,
                                       std::vector<int> PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kBeam,
                                     ProSelecta_detail::kFirst>(ev, PIDs);
}

// GetBeams(event, PID) -> list<particle>
std::vector<HepMC3::ConstGenParticlePtr> GetBeams(HepMC3::GenEvent const &ev,
                                                  int PID) {
  return ProSelecta_detail::particles_1pdg<ProSelecta_detail::kBeam,
                                           ProSelecta_detail::kFromPDGList>(
      ev, PID);
}

// GetBeamsAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr> GetBeamsAny(HepMC3::GenEvent const &ev,
                                                     std::vector<int> PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam,
                                      ProSelecta_detail::kFromPDGList>(ev,
                                                                       PIDs);
}

// GetTarget(event) -> particle
HepMC3::ConstGenParticlePtr GetTarget(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::particle_any<ProSelecta_detail::kTarget,
                                         ProSelecta_detail::kFirst>(ev);
}

// GetOutPartFirst(event, PID) -> particle
HepMC3::ConstGenParticlePtr GetOutPartFirst(HepMC3::GenEvent const &ev,
                                            int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kUndecayedPhysical,
                                          ProSelecta_detail::kFirst>(ev, PID);
}
// GetOutPartFirstAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr GetOutPartFirstAny(HepMC3::GenEvent const &ev,
                                               std::vector<int> PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kUndecayedPhysical,
                                     ProSelecta_detail::kFirst>(ev, PIDs);
}

// GetOutPartHM(event, PID) -> particle
HepMC3::ConstGenParticlePtr GetOutPartHM(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particle_1pdg<ProSelecta_detail::kUndecayedPhysical,
                                          ProSelecta_detail::kHighestMomentum>(
      ev, PID);
}

// GetOutPartHMAny(event, list<PID>) -> particle
HepMC3::ConstGenParticlePtr GetOutPartHMAny(HepMC3::GenEvent const &ev,
                                            std::vector<int> PIDs) {
  return ProSelecta_detail::particle<ProSelecta_detail::kUndecayedPhysical,
                                     ProSelecta_detail::kHighestMomentum>(ev,
                                                                          PIDs);
}

// GetOutParts(event, PID) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr> GetOutParts(HepMC3::GenEvent const &ev,
                                                     int PID) {
  return ProSelecta_detail::particles_1pdg<
      ProSelecta_detail::kUndecayedPhysical, ProSelecta_detail::kFromPDGList>(
      ev, PID);
}
// GetOutPartsAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
GetOutPartsAny(HepMC3::GenEvent const &ev, std::vector<int> PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kFromPDGList>(ev,
                                                                       PIDs);
}

// GetOutPartsExcept(event, PID) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
GetOutPartsExcept(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particles_1pdg<
      ProSelecta_detail::kUndecayedPhysical,
      ProSelecta_detail::kNotFromPDGList>(ev, PID);
}
// GetOutPartsExceptAny(event, list<PID>) -> list<particles>
std::vector<HepMC3::ConstGenParticlePtr>
GetOutPartsExceptAny(HepMC3::GenEvent const &ev, std::vector<int> PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs);
}

} // namespace ps