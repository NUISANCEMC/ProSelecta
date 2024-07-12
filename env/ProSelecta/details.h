#pragma once
#include "ProSelecta/pdg.h"
#include "ProSelecta/vect.h"

#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"

#include <string>
#include <vector>

namespace ps {
namespace part {

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
    cuts out = *this;
    out.negate = !out.negate;
    return out;
  }
};
} // namespace part

template <typename Projector> struct cutable {
  part::cuts operator<(double lim) const {
    return part::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) < lim;
        },
    }};
  }
  part::cuts operator>(double lim) const {
    return part::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) > lim;
        },
    }};
  }
  part::cuts operator<=(double lim) const {
    return part::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) <= lim;
        },
    }};
  }
  part::cuts operator>=(double lim) const {
    return part::cuts{{
        [=](HepMC3::ConstGenParticlePtr part) -> bool {
          return static_cast<Projector const *>(this)->operator()(part) >= lim;
        },
    }};
  }
};

struct p3mod_ : public cutable<p3mod_> {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum().p3mod();
  }
} p3mod;

struct energy_ : public cutable<energy_> {
  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum().e();
  }
} energy;

struct theta_ : public cutable<theta_> {

  HepMC3::FourVector refv = HepMC3::FourVector{0, 0, 1, 0};

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return vect::angle(part->momentum(), refv);
  }

  theta_ operator()(HepMC3::FourVector const &refvec) {
    theta_ proj;
    proj.refv = refvec;
    return proj;
  }

} theta;

struct costheta_ : public cutable<costheta_> {

  HepMC3::FourVector refv = HepMC3::FourVector{0, 0, 1, 0};

  double operator()(HepMC3::ConstGenParticlePtr part) const {
    return std::cos(vect::angle(part->momentum(), refv));
  }

  costheta_ operator()(HepMC3::FourVector const &refvec) {
    costheta_ proj;
    proj.refv = refvec;
    return proj;
  }

} costheta;

struct momentum_ {
  HepMC3::FourVector operator()(HepMC3::ConstGenParticlePtr part) const {
    return part->momentum();
  }
} momentum;

} // namespace ps

namespace ProSelecta_detail {

template <typename Collection> struct is_zero_std_array : std::true_type {};

template <size_t N> struct is_zero_std_array<std::array<int, N>> {
  constexpr static bool value = (N > 0);
};

template <typename Collection> struct is_std_array_int : std::false_type {};
template <size_t N>
struct is_std_array_int<std::array<int, N>> : std::true_type {};

template <typename Collection> struct is_std_vector_int : std::false_type {};
template <> struct is_std_vector_int<std::vector<int>> : std::true_type {};

template <typename Collection> struct is_std_vector_or_array_int {
  constexpr static bool value = is_std_array_int<Collection>::value ||
                                is_std_vector_int<Collection>::value;
};

template <typename Collection, typename ValueType> struct broadcast_return {};

template <size_t N, typename ValueType>
struct broadcast_return<std::array<int, N>, ValueType> {
  using type = std::array<ValueType, N>;
};

template <typename ValueType>
struct broadcast_return<std::vector<int>, ValueType> {
  using type = std::vector<ValueType>;
};

struct exception : public std::exception {
  std::string msg;

  exception(std::string const &m) : msg(m) {}
  const char *what() const noexcept { return msg.c_str(); }
};

constexpr int kUndecayedPhysical = 1;
constexpr int kBeam = 4;
constexpr int kTarget = 11;

constexpr bool kFromPDGList = true;
constexpr bool kNotFromPDGList = false;

template <int status, typename Collection, bool select_from_pdg_list = true>
std::vector<HepMC3::ConstGenParticlePtr> particles(HepMC3::GenEvent const &evt,
                                                   Collection const &pdgs) {

  std::vector<HepMC3::ConstGenParticlePtr> selected_parts = {};

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }

    if constexpr (status == kUndecayedPhysical) {
      if (part->pid() >= ps::pdg::kNuclearPDGBoundary) {
        continue;
      }
    }

    if constexpr (std::is_same_v<Collection, std::array<int, 0>>) {
      selected_parts.push_back(part);
    } else if (std::is_same_v<Collection, std::vector<int>> &&
               (pdgs.size() == 0)) {
      selected_parts.push_back(part);
    } else if ((std::find(pdgs.begin(), pdgs.end(), part->pid()) !=
                pdgs.end()) == select_from_pdg_list) {
      selected_parts.push_back(part);
    }
  }
  return selected_parts;
}

template <int status, typename Collection>
bool has_particles(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  bool hasall = true;

  for (auto id : PIDs) {
    hasall = hasall && bool(particles<status>(ev, std::array{id}).size());
  }

  return hasall;
}

template <int status, typename Collection>
bool has_particles_exact(HepMC3::GenEvent const &ev, Collection const &PIDs,
                         Collection const &counts) {
  bool hasall = true;

  for (size_t i = 0; i < PIDs.size(); ++i) {
    hasall = hasall && bool(particles<status>(ev,
                                              std::array{
                                                  PIDs[i],
                                              })
                                .size() == counts[i]);
  }

  return hasall;
}

template <int status, typename Collection>
bool has_particles_atleast(HepMC3::GenEvent const &ev, Collection const &PIDs,
                           Collection const &counts) {
  bool hasall = true;

  for (size_t i = 0; i < PIDs.size(); ++i) {
    hasall = hasall && bool(particles<status>(ev,
                                              std::array{
                                                  PIDs[i],
                                              })
                                .size() >= counts[i]);
  }

  return hasall;
}

template <int status>
std::vector<HepMC3::ConstGenParticlePtr>
nuclear_particles(HepMC3::GenEvent const &evt) {

  std::vector<HepMC3::ConstGenParticlePtr> selected_parts = {};

  for (auto const &part : evt.particles()) {
    if (part->status() != status) {
      continue;
    }
    if (part->pid() < ps::pdg::kNuclearPDGBoundary) {
      continue;
    }

    selected_parts.push_back(part);
  }
  return selected_parts;
}

} // namespace ProSelecta_detail