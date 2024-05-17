#pragma once

#include "ProSelecta/pdg.h"
#include "ProSelecta/vect.h"
#include "ProSelecta/unit.h"
#include "ProSelecta/missing_datum.h"

namespace ps {
namespace part {

// parts::q0(particle, particle) -> real
double q0(HepMC3::ConstGenParticlePtr pin, HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return ps::kMissingDatum<double>;
  }

  return (pin->momentum() - pout->momentum()).e();
}

// parts::q3(particle, particle) -> real
double q3(HepMC3::ConstGenParticlePtr pin, HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return ps::kMissingDatum<double>;
  }

  return (pin->momentum() - pout->momentum()).p3mod();
}

// parts::Q2(particle, particle) -> real
double Q2(HepMC3::ConstGenParticlePtr pin,
             HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return ps::kMissingDatum<double>;
  }

  return -(pin->momentum() - pout->momentum()).m2();
}

// parts::CosTheta(particle, particle) -> real
double CosTheta(HepMC3::ConstGenParticlePtr p1,
                HepMC3::ConstGenParticlePtr p2) {
  if (!p1 || !p2) {
    return ps::kMissingDatum<double>;
  }

  return cos(ps::vect::angle(p1->momentum(), p2->momentum()));
}

// parts::Theta(particle, particle) -> real
double Theta(HepMC3::ConstGenParticlePtr p1, HepMC3::ConstGenParticlePtr p2) {
  if (!p1 || !p2) {
    return ps::kMissingDatum<double>;
  }
  
  return (ps::vect::angle(p1->momentum(), p2->momentum()));
}

// parts::W(list<particles>) -> real
double W(std::vector<HepMC3::ConstGenParticlePtr> parts) {
  HepMC3::FourVector fv;

  for (auto &p : parts) {
    if (!p) {
      return ps::kMissingDatum<double>;
    }
    fv += p->momentum();
  }

  return fv.m();
}

// parts::EPmiss(list<particles>) -> 4vec
HepMC3::FourVector EPmiss(std::vector<HepMC3::ConstGenParticlePtr> parts_in,
                          std::vector<HepMC3::ConstGenParticlePtr> parts_out) {
  HepMC3::FourVector fv_in, fv_out;

  for (auto &p : parts_in) {
    if (!p) {
      return HepMC3::FourVector{ps::kMissingDatum<double>, 0, 0, 0};
    }
    fv_in += p->momentum();
  }

  for (auto &p : parts_out) {
    if (!p) {
      return HepMC3::FourVector{ps::kMissingDatum<double>, 0, 0, 0};
    }
    fv_out += p->momentum();
  }

  return fv_in - fv_out;
}

// parts::Pt(list<particles>) -> real
double Pt(std::vector<HepMC3::ConstGenParticlePtr> parts) {
  HepMC3::FourVector fv_tot;

  for (auto &p : parts) {
    if (!p) {
      return ps::kMissingDatum<double>;
    }
    fv_tot += p->momentum();
  }

  return fv_tot.perp();
}

} // namespace part
} // namespace ps