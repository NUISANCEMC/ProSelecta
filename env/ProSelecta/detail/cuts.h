#pragma once

#include "HepMC3/GenParticle.h"

#include "ProSelecta/cuts.h"

#include <string>
#include <vector>

namespace ps::detail {

template <typename Projector> struct cutable {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return Projector::project(part);
  }

  ps::cuts operator<(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return Projector::project(part) < lim;
        },
    }};
  }
  ps::cuts operator>(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return Projector::project(part) > lim;
        },
    }};
  }
  ps::cuts operator<=(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return Projector::project(part) <= lim;
        },
    }};
  }
  ps::cuts operator>=(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return Projector::project(part) >= lim;
        },
    }};
  }
};

} // namespace ps::detail