#pragma once

#include "ProSelecta/missing_datum.h"
#include "ProSelecta/pdg.h"
#include "ProSelecta/unit.h"
#include "ProSelecta/vect.h"

namespace ps {

namespace part {

struct EmptyParticleList : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};

struct NoParts : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct TooManyParts : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};

template <typename T>
auto sortascendingby(T const &projector,
                     std::vector<HepMC3::ConstGenParticlePtr> parts) {
  if (!parts.size()) {
    throw EmptyParticleList("sort: no particles");
  }
  std::sort(parts.begin(), parts.end(),
            [=](HepMC3::ConstGenParticlePtr a, HepMC3::ConstGenParticlePtr b) {
              return projector(a) < projector(b);
            });
  return parts;
}

template <typename T>
auto highest(T const &projector,
             std::vector<HepMC3::ConstGenParticlePtr> parts) {
  if (!parts.size()) {
    throw EmptyParticleList("highest: no particles");
  }
  return sortascendingby(projector, parts).back();
}

template <typename T>
auto lowest(T const &projector,
            std::vector<HepMC3::ConstGenParticlePtr> parts) {
  if (!parts.size()) {
    throw EmptyParticleList("lowest: no particles");
  }
  return sortascendingby(projector, parts).front();
}

auto filter(cuts const &c, std::vector<HepMC3::ConstGenParticlePtr> parts) {
  parts.erase(std::remove_if(parts.begin(), parts.end(), std::not1(c)),
              parts.end());
  return parts;
}

auto one(std::vector<HepMC3::ConstGenParticlePtr> parts) {
  if (!parts.size()) {
    throw NoParts("ps::part::one passed an empty vector");
  }
  if (parts.size() > 1) {
    throw TooManyParts("ps::part::one passed more than one particle");
  }
  return parts.front();
}

template <size_t N> auto one(std::array<HepMC3::ConstGenParticlePtr, N> parts) {

  HepMC3::ConstGenParticlePtr part = nullptr;
  for (auto const &arr : parts) {
    if (arr.size() > 1) {
      throw TooManyParts(
          "ps::part::one passed an array with more than one particle");
    } else if (arr.size() == 1) {
      if (!part) {
        part = arr.front();
      } else {
        throw TooManyParts(
            "ps::part::one passed an array with more than one particle");
      }
    }
  }

  if (part) {
    throw NoParts("ps::part::one passed an empty array of vectors");
  }

  return part;
}

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
double Q2(HepMC3::ConstGenParticlePtr pin, HepMC3::ConstGenParticlePtr pout) {
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