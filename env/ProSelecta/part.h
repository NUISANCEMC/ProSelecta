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
auto sort_ascending(T const &projector,
                    std::vector<HepMC3::ConstGenParticlePtr> parts) {
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
  return sort_ascending(projector, parts).back();
}

template <typename T>
auto lowest(T const &projector,
            std::vector<HepMC3::ConstGenParticlePtr> parts) {
  if (!parts.size()) {
    throw EmptyParticleList("lowest: no particles");
  }
  return sort_ascending(projector, parts).front();
}

auto filter(cuts const &c, std::vector<HepMC3::ConstGenParticlePtr> parts) {
  parts.erase(std::remove_if(parts.begin(), parts.end(), !c), parts.end());
  return parts;
}

template <size_t N>
auto cat(std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts) {

  std::vector<HepMC3::ConstGenParticlePtr> all_parts;
  for (auto const &arr : parts) {
    std::copy(arr.begin(), arr.end(), std::back_inserter(all_parts));
  }

  return all_parts;
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

template <size_t N>
auto one(std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts) {
  return one(cat(parts));
}

template <typename T>
auto sum(T const &projector, std::vector<HepMC3::ConstGenParticlePtr> parts) {
  return std::accumulate(
      parts.begin(), parts.end(), decltype(projector(parts.front())){},
      [&](auto const &tot, auto const &part) { return tot + projector(part); });
}

template <typename T, size_t N>
auto sum(T const &projector,
         std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts) {
  return std::accumulate(
      parts.begin(), parts.end(), decltype(projector(parts.front().front())){},
      [&](auto const &all_tot, auto const &partarr) {
        return all_tot +
               std::accumulate(partarr.begin(), partarr.end(),
                               decltype(projector(parts.front().front())){},
                               [&](auto const &part_tot, auto const &part) {
                                 return part_tot + projector(part);
                               });
      });
}

} // namespace part
} // namespace ps