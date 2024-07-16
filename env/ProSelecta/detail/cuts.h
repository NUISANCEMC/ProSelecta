#pragma once

#include "HepMC3/GenParticle.h"

#include "ProSelecta/cuts.h"

#include <string>
#include <vector>

namespace ps::detail {

template <typename Projector> struct cutable {
  ps::cuts operator<(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) < lim;
        },
    }};
  }
  ps::cuts operator>(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) > lim;
        },
    }};
  }
  ps::cuts operator<=(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) <= lim;
        },
    }};
  }
  ps::cuts operator>=(double lim) const {
    return ps::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) >= lim;
        },
    }};
  }
};

} // namespace ps::detail