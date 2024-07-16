#pragma once

#include "ProSelecta/detail/except.h"
#include "ProSelecta/detail/part.h"
#include "ProSelecta/detail/projectors.h"

#include <numeric>

namespace ps {

static ps::detail::p3mod p3mod;
static ps::detail::energy energy;
static ps::detail::kinetic_energy kinetic_energy;
static ps::detail::theta theta;
static ps::detail::costheta costheta;
static ps::detail::momentum momentum;

static ps::detail::squeeze squeeze;

namespace part {

NEW_PS_EXCEPT(EmptyParticleList);
NEW_PS_EXCEPT(NoParts);
NEW_PS_EXCEPT(TooManyParts);
NEW_PS_EXCEPT(InvalidProjector);

template <typename T, typename Collection>
inline auto sort_ascending(T const &projector, Collection parts) {

  static_assert(
      ps::detail::is_std_vector_or_array_part<Collection>::value,
      "parts type must be a std::array<HepMC3::ConstGenParticlePtr,N> or "
      "std::vector<HepMC3::ConstGenParticlePtr>");

  std::sort(parts.begin(), parts.end(),
            [=](HepMC3::ConstGenParticlePtr a, HepMC3::ConstGenParticlePtr b) {
              return projector(a) < projector(b);
            });
  return parts;
}

template <typename T, typename PartCollectionCollection>
inline auto sort_ascending(T const &projector, PartCollectionCollection parts,
                           ps::detail::squeeze const &) {
  return sort_ascending(projector, ps::detail::cat(parts));
}

template <typename T, typename PartCollectionCollection>
inline auto highest(T const &projector, PartCollectionCollection parts) {
  if constexpr (ps::detail::is_std_vector_or_array_part<
                    PartCollectionCollection>::value) {
    if (!parts.size()) {
      throw EmptyParticleList("highest: no particles");
    }
    return sort_ascending(projector, parts).back();
  } else {
    if constexpr (ps::detail::is_std_array<PartCollectionCollection>::value) {
      std::array<HepMC3::ConstGenParticlePtr,
                 std::tuple_size<PartCollectionCollection>::value>
          outs;
      for (size_t i = 0; i < parts.size(); ++i) {
        if (!parts[i].size()) {
          std::stringstream ss;
          ss << "highest: no particles in array sub-vector, array entry: " << i;
          throw EmptyParticleList(ss.str());
        }
        outs[i] = highest(projector, parts[i]);
      }

      return outs;
    } else if constexpr (ps::detail::is_std_vector<
                             PartCollectionCollection>::value) {
      std::vector<HepMC3::ConstGenParticlePtr> outs;
      for (size_t i = 0; i < parts.size(); ++i) {
        if (!parts[i].size()) {
          std::stringstream ss;
          ss << "highest: no particles in vector sub-vector, vector entry: "
             << i;
          throw EmptyParticleList(ss.str());
        }
        outs.push_back(highest(projector, parts[i]));
      }

      return outs;
    }
  }
}

template <typename T, typename PartCollectionCollection>
inline auto highest(T const &projector, PartCollectionCollection parts,
                    ps::detail::squeeze const &) {
  auto all_parts = ps::detail::cat(parts);
  if (!all_parts.size()) {
    throw EmptyParticleList("highest: no particles");
  }
  return sort_ascending(projector, all_parts).back();
}

template <typename T, typename PartCollectionCollection>
inline auto lowest(T const &projector, PartCollectionCollection parts) {
  if constexpr (ps::detail::is_std_vector_or_array_part<
                    PartCollectionCollection>::value) {
    if (!parts.size()) {
      throw EmptyParticleList("lowest: no particles");
    }
    return sort_ascending(projector, parts).front();
  } else {
    if constexpr (ps::detail::is_std_array<PartCollectionCollection>::value) {
      std::array<HepMC3::ConstGenParticlePtr,
                 std::tuple_size<PartCollectionCollection>::value>
          outs;
      for (size_t i = 0; i < parts.size(); ++i) {
        if (!parts[i].size()) {
          std::stringstream ss;
          ss << "lowest: no particles in array sub-vector, array entry: " << i;
          throw EmptyParticleList(ss.str());
        }
        outs[i] = lowest(projector, parts[i]);
      }

      return outs;
    } else if constexpr (ps::detail::is_std_vector<
                             PartCollectionCollection>::value) {
      std::vector<HepMC3::ConstGenParticlePtr> outs;
      for (size_t i = 0; i < parts.size(); ++i) {
        if (!parts[i].size()) {
          std::stringstream ss;
          ss << "lowest: no particles in vector sub-vector, vector entry: "
             << i;
          throw EmptyParticleList(ss.str());
        }
        outs.push_back(lowest(projector, parts[i]));
      }

      return outs;
    }
  }
}

template <typename T, typename PartCollectionCollection>
inline auto lowest(T const &projector, PartCollectionCollection parts,
                   ps::detail::squeeze const &) {
  auto all_parts = ps::detail::cat(parts);
  if (!all_parts.size()) {
    throw EmptyParticleList("lowest: no particles");
  }
  return sort_ascending(projector, all_parts).front();
}

template <typename PartCollectionCollection>
inline auto filter(ps::cuts const &c, PartCollectionCollection parts) {

  if constexpr (ps::detail::is_std_array_part<
                    PartCollectionCollection>::value) {
    std::vector<HepMC3::ConstGenParticlePtr> outs;
    for (auto const &p : parts) {
      if (c(p)) {
        outs.push_back(p);
      }
    }
    return outs;
  } else if constexpr (ps::detail::is_std_vector_part<
                           PartCollectionCollection>::value) {
    parts.erase(std::remove_if(parts.begin(), parts.end(), !c), parts.end());
    return parts;
  } else {

    for (size_t i = 0; i < parts.size(); ++i) {
      parts[i].erase(std::remove_if(parts[i].begin(), parts[i].end(), !c),
                     parts[i].end());
    }

    return parts;
  }
}

template <typename PartCollectionCollection>
inline auto filter(ps::cuts const &c,
                   PartCollectionCollection const &part_groups,
                   ps::detail::squeeze const &) {

  return filter(c, ps::detail::cat(part_groups));
}

template <typename T, typename PartCollectionCollection>
inline auto sum(T const &projector, PartCollectionCollection const &parts) {

  if constexpr (ps::detail::is_std_vector_or_array_part<
                    PartCollectionCollection>::value) {
    return std::accumulate(parts.begin(), parts.end(),
                           decltype(projector(parts.front())){},
                           [&](auto const &tot, auto const &part) {
                             return tot + projector(part);
                           });
  } else {

    if constexpr (ps::detail::is_std_array<PartCollectionCollection>::value) {
      std::array<decltype(projector(parts.front().front())),
                 std::tuple_size<PartCollectionCollection>::value>
          outs;
      for (size_t i = 0; i < parts.size(); ++i) {
        outs[i] = std::accumulate(parts[i].begin(), parts[i].end(),
                                  decltype(projector(parts.front().front())){},
                                  [&](auto const &part_tot, auto const &part) {
                                    return part_tot + projector(part);
                                  });
      }

      return outs;
    } else if constexpr (ps::detail::is_std_vector<
                             PartCollectionCollection>::value) {
      std::vector<decltype(projector(parts.front().front()))> outs;
      for (auto const &part_groups : parts) {
        outs.push_back(
            std::accumulate(part_groups.begin(), part_groups.end(),
                            decltype(projector(parts.front().front())){},
                            [&](auto const &part_tot, auto const &part) {
                              return part_tot + projector(part);
                            }));
      }

      return outs;
    }
  }
}

template <typename T, typename PartCollectionCollection>
inline auto sum(T const &projector, PartCollectionCollection const &parts,
                ps::detail::squeeze const &) {
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