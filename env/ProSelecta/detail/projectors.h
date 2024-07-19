#pragma once

#include "ProSelecta/detail/cuts.h"

#include "ProSelecta/vect.h"

#include "HepMC3/FourVector.h"
#include "HepMC3/GenParticle.h"

#include <cmath>
#include <string>
#include <vector>

namespace ps::detail {

struct p3mod : public cutable<p3mod> {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum().p3mod();
  }
};

struct energy : public cutable<energy> {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum().e();
  }
};

struct kinetic_energy : public cutable<kinetic_energy> {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum().e() - part->momentum().m();
  }
};

struct theta : public cutable<theta> {

  HepMC3::FourVector refv = HepMC3::FourVector{0, 0, 1, 0};

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return vect::angle(part->momentum(), refv);
  }

  theta operator()(HepMC3::FourVector const &refvec) const {
    theta proj;
    proj.refv = refvec;
    return proj;
  }
};

struct costheta : public cutable<costheta> {

  HepMC3::FourVector refv = HepMC3::FourVector{0, 0, 1, 0};

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return std::cos(vect::angle(part->momentum(), refv));
  }

  costheta operator()(HepMC3::FourVector const &refvec) const {
    costheta proj;
    proj.refv = refvec;
    return proj;
  }
};

struct momentum {
  HepMC3::FourVector operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum();
  }
};

} // namespace ps::detail