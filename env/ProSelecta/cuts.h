#pragma once

#include "HepMC3/GenParticle.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace ps {

struct cuts {
  using argument_type = HepMC3::ConstGenParticlePtr;

  std::vector<std::function<bool(HepMC3::ConstGenParticlePtr)>> fcuts;
  bool negate = false;

  bool operator()(HepMC3::ConstGenParticlePtr part) const {
    bool passall = true;

    for (auto &cut : fcuts) {
      passall = passall && cut(part);
    }

    return negate ? !passall : passall;
  }

  cuts operator&&(cuts const &other) const {
    cuts out = *this;
    std::copy(other.fcuts.begin(), other.fcuts.end(),
              std::back_inserter(out.fcuts));
    return out;
  }

  cuts operator!() const {
    return cuts{fcuts,!negate};
  }
};

} // namespace ps
