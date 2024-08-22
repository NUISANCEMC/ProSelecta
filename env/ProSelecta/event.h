#pragma once

#include "ProSelecta/detail/TMP.h"
#include "ProSelecta/detail/event.h"
#include "ProSelecta/detail/except.h"
#include "ProSelecta/detail/part.h"

#include "ProSelecta/part.h"

#include "HepMC3/GenEvent.h"

#include <stdexcept>

namespace ps {
namespace event {

NEW_PS_EXCEPT(EmptyPIDList);
NEW_PS_EXCEPT(MismatchedPIDAndCountsListLength);
NEW_PS_EXCEPT(NoMatchingParts);
NEW_PS_EXCEPT(MoreThanOneBeamPart);
NEW_PS_EXCEPT(MoreThanOneTargetPart);
NEW_PS_EXCEPT(NoSignalProcessId);

template <typename Collection>
inline auto num_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {

  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "num_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("num_out_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ps::detail::broadcast_return<Collection, int>::type outs;

  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] = ps::detail::particles<ps::detail::kUndecayedPhysical>(
                  ev, std::array{PIDs[i]})
                  .size();
  }
  return outs;
}

template <typename Collection>
inline auto num_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs,
                         ps::detail::flatten const &) {

  auto const &all_num_out_part = num_out_part(ev, PIDs);

  return std::accumulate(all_num_out_part.begin(), all_num_out_part.end(), 0);
}

inline int num_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ps::detail::particles<ps::detail::kUndecayedPhysical>(
               ev, std::array{PID})
        .size();
  }

  return ps::detail::particles<ps::detail::kUndecayedPhysical>(
             ev, std::array<int, 0>{})
      .size();
}

template <typename Collection>
inline int num_out_part_except(HepMC3::GenEvent const &ev,
                               Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "num_out_part_except: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("num_out_part_except passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ps::detail::particles<ps::detail::kUndecayedPhysical, Collection,
                               ps::detail::kNotFromPDGList>(ev, PIDs)
      .size();
}

inline int num_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return num_out_part_except(ev, std::array{PID});
}

template <typename Collection>
inline bool has_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "has_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_out_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  for (auto const &PID : PIDs) {
    if (ps::detail::has_particles<ps::detail::kUndecayedPhysical>(
            ev, std::array{PID})) {
      return true;
    }
  }
  return false;
}

inline bool has_out_part(HepMC3::GenEvent const &ev, int PID) {
  return ps::detail::has_particles<ps::detail::kUndecayedPhysical>(
      ev, std::array{PID});
}

template <typename Collection>
inline bool has_exact_out_part(HepMC3::GenEvent const &ev,
                               Collection const &PIDs,
                               Collection const &counts) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "has_exact_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_exact_out_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ps::detail::has_particles_exact<ps::detail::kUndecayedPhysical>(
      ev, PIDs, counts);
}

inline bool has_exact_out_part(HepMC3::GenEvent const &ev, int PID, int count) {
  return ps::detail::has_particles_exact<ps::detail::kUndecayedPhysical>(
      ev, std::array{PID}, {count});
}

template <typename Collection>
inline bool out_part_topology_matches(HepMC3::GenEvent const &ev,
                                      Collection const &PIDs,
                                      Collection const &counts) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "out_part_topology_matches: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("out_part_topology_matches passed empty PID list");
    }
    if (PIDs.size() != counts.size()) {
      std::stringstream ss;
      ss << "out_part_topology_matches passed " << PIDs.size() << "PIDs and "
         << counts.size() << " counts. These must match";
      throw MismatchedPIDAndCountsListLength(ss.str());
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return has_exact_out_part(ev, PIDs, counts) &&
         (ps::detail::particles<ps::detail::kUndecayedPhysical>(
              ev, std::array<int, 0>{})
              .size() == std::accumulate(counts.begin(), counts.end(), 0));
}

template <typename Collection>
inline bool has_at_least_out_part(HepMC3::GenEvent const &ev,
                                  Collection const &PIDs,
                                  Collection const &counts) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "has_at_least_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_at_least_out_part passed empty PID list");
    }
    if (PIDs.size() != counts.size()) {
      std::stringstream ss;
      ss << "has_at_least_out_part passed " << PIDs.size() << "PIDs and "
         << counts.size() << " counts. These must match";
      throw MismatchedPIDAndCountsListLength(ss.str());
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ps::detail::has_particles_atleast<ps::detail::kUndecayedPhysical>(
      ev, PIDs, counts);
}

inline bool has_at_least_out_part(HepMC3::GenEvent const &ev, int PID,
                                  int count) {
  return ps::detail::has_particles_atleast<ps::detail::kUndecayedPhysical>(
      ev, std::array{PID}, {count});
}

template <typename Collection>
inline auto all_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "all_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("all_out_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ps::detail::broadcast_return<
      Collection, std::vector<HepMC3::ConstGenParticlePtr>>::type outs;

  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] = ps::detail::particles<ps::detail::kUndecayedPhysical>(
        ev, std::array{PIDs[i]});
  }
  return outs;
}

template <typename Collection>
inline auto all_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs,
                         ps::detail::flatten const &) {
  return ps::detail::particles<ps::detail::kUndecayedPhysical>(ev, PIDs);
}

inline std::vector<HepMC3::ConstGenParticlePtr>
all_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ps::detail::particles<ps::detail::kUndecayedPhysical>(
        ev, std::array{PID});
  }

  return ps::detail::particles<ps::detail::kUndecayedPhysical>(
      ev, std::array<int, 0>{});
}

template <typename Collection>
inline auto hm_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "hm_out_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("hm_out_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ps::detail::broadcast_return<Collection,
                                        HepMC3::ConstGenParticlePtr>::type outs;

  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] = ps::part::highest(
        ps::p3mod, ps::detail::particles<ps::detail::kUndecayedPhysical>(
                       ev, std::array{PIDs[i]}));
  }
  return outs;
}

template <typename Collection>
inline auto hm_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs,
                        ps::detail::flatten const &) {
  return ps::part::highest(ps::p3mod, hm_out_part(ev, PIDs));
}

inline HepMC3::ConstGenParticlePtr hm_out_part(HepMC3::GenEvent const &ev,
                                               int PID) {
  return ps::part::highest(
      ps::p3mod, ps::detail::particles<ps::detail::kUndecayedPhysical>(
                     ev, std::array{PID}));
}

template <typename Collection>
inline auto all_out_part_except(HepMC3::GenEvent const &ev,
                                Collection const &PIDs) {
  return ps::detail::particles<ps::detail::kUndecayedPhysical, Collection,
                               ps::detail::kNotFromPDGList>(ev, PIDs);
}

inline auto all_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return ps::detail::particles<ps::detail::kUndecayedPhysical,
                               std::array<int, 1>, ps::detail::kNotFromPDGList>(
      ev, std::array{PID});
}

template <typename Collection>
inline bool has_beam_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {

  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "has_beam_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_beam_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ps::detail::particles<ps::detail::kBeam>(ev, PIDs).size();
}

inline bool has_beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ps::detail::has_particles<ps::detail::kBeam>(ev, std::array{PID});
  }
  return ps::detail::particles<ps::detail::kBeam>(ev, std::array<int, 0>{})
      .size();
}

inline auto beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ps::detail::particles<ps::detail::kBeam>(ev, std::array{PID})
          : ps::detail::particles<ps::detail::kBeam>(ev, std::array<int, 0>{});

  if (!parts.size()) {
    std::stringstream ss;
    ss << "beam_part(" << PID << "): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "beam_part(" << PID << "): MoreThanOneBeamPart";
    throw MoreThanOneBeamPart(ss.str());
  }
  return parts.front();
}

template <typename Collection>
inline auto beam_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "beam_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("beam_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  auto parts = ps::detail::particles<ps::detail::kBeam>(ev, PIDs);

  if (!parts.size()) {
    std::stringstream ss;
    ss << "beam_part({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "beam_part({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): MoreThanOneBeamPart";
    throw MoreThanOneBeamPart(ss.str());
  }
  return parts.front();
}

template <typename Collection>
inline bool has_target_part(HepMC3::GenEvent const &ev,
                            Collection const &PIDs) {

  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "has_target_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_target_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ps::detail::particles<ps::detail::kTarget>(ev, PIDs).size();
}

inline bool has_target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ps::detail::has_particles<ps::detail::kTarget>(ev, std::array{PID});
  }
  return ps::detail::particles<ps::detail::kTarget>(ev, std::array<int, 0>{})
      .size();
}

inline auto target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ps::detail::particles<ps::detail::kTarget>(ev, std::array{PID})
          : ps::detail::particles<ps::detail::kTarget>(ev,
                                                       std::array<int, 0>{});
  if (!parts.size()) {
    std::stringstream ss;
    ss << "target_part(" << PID << "): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "target_part(" << PID << "): MoreThanOneTargetPart";
    throw MoreThanOneTargetPart(ss.str());
  }
  return parts.front();
}

template <typename Collection>
inline auto target_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(!ps::detail::is_zero_std_array<Collection>::value,
                "target_part: EmptyPIDList");
  if constexpr (ps::detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("target_part passed empty PID list");
    }
  }

  static_assert(ps::detail::is_std_vector_or_array_int<Collection>::value,
                "PIDs type must be a std::array<int,N> or std::vector<int>");

  auto parts = ps::detail::particles<ps::detail::kTarget>(ev, PIDs);

  if (!parts.size()) {
    std::stringstream ss;
    ss << "target_part({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "target_part({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): MoreThanOneTargetPart";
    throw MoreThanOneTargetPart(ss.str());
  }
  return parts.front();
}

inline auto out_nuclear_parts(HepMC3::GenEvent const &ev) {
  return ps::detail::nuclear_particles<ps::detail::kUndecayedPhysical>(ev);
}

inline int signal_process_id(HepMC3::GenEvent const &ev) {

  auto const &attr_names = ev.attribute_names();
  if (std::find(attr_names.begin(), attr_names.end(), "signal_process_id") ==
      attr_names.end()) {
    throw NoSignalProcessId("Event contains no signal_process_id attribute");
  }

  return ev.attribute<HepMC3::IntAttribute>("signal_process_id")->value();
}

} // namespace event
} // namespace ps
