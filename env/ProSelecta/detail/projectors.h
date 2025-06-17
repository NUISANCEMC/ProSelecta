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
  static double project(HepMC3::ConstGenParticlePtr part) {
    return part->momentum().p3mod();
  }
};

struct energy : public cutable<energy> {
  static double project(HepMC3::ConstGenParticlePtr part) {
    return part->momentum().e();
  }
};

struct kinetic_energy : public cutable<kinetic_energy> {
  static double project(HepMC3::ConstGenParticlePtr part) {
    return part->momentum().e() - part->momentum().m();
  }
};

struct theta {

  HepMC3::FourVector refv;

  theta() : refv{HepMC3::FourVector{0, 0, 1, 0}} {}
  theta(theta const &other) : refv{other.refv} {}

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return vect::angle(part->momentum(), refv);
  }

  theta operator()(HepMC3::FourVector const &refvec) const {
    theta proj;
    proj.refv = refvec;
    return proj;
  }

  // need to make sure the lambda owns a copy of the refv or if the theta
  // instance goes out of scope before the cuts we have a danling this reference
  // from cutable.
  ps::cuts operator<(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return vect::angle(part->momentum(), xrefv) < lim;
        },
    }};
  }
  ps::cuts operator>(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return vect::angle(part->momentum(), xrefv) > lim;
        },
    }};
  }
  ps::cuts operator<=(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return vect::angle(part->momentum(), xrefv) <= lim;
        },
    }};
  }
  ps::cuts operator>=(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return vect::angle(part->momentum(), xrefv) >= lim;
        },
    }};
  }
};

struct costheta {

  HepMC3::FourVector refv = HepMC3::FourVector{0, 0, 1, 0};

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return std::cos(vect::angle(part->momentum(), refv));
  }

  costheta operator()(HepMC3::FourVector const &refvec) const {
    costheta proj;
    proj.refv = refvec;
    return proj;
  }

  // need to make sure the lambda owns a copy of the refv or if the theta
  // instance goes out of scope before the cuts we have a danling this reference
  // from cutable.
  ps::cuts operator<(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return std::cos(vect::angle(part->momentum(), xrefv)) < lim;
        },
    }};
  }
  ps::cuts operator>(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return std::cos(vect::angle(part->momentum(), xrefv)) > lim;
        },
    }};
  }
  ps::cuts operator<=(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return std::cos(vect::angle(part->momentum(), xrefv)) <= lim;
        },
    }};
  }
  ps::cuts operator>=(double lim) const {
    HepMC3::FourVector xrefv = refv;
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return std::cos(vect::angle(part->momentum(), xrefv)) >= lim;
        },
    }};
  }
};

struct momentum {
  HepMC3::FourVector operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum();
  }
};

} // namespace ps::detail