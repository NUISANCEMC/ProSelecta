#pragma once

#include "ProSelecta/details.h"
#include "ProSelecta/part.h"

#include <stdexcept>

namespace ps {
namespace event {

struct EmptyPIDList : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct MismatchedPIDAndCountsListLength : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct NoMatchingParts : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct MoreThanOneBeamPart : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct MoreThanOneTargetPart : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};
struct NoSignalProcessId : public ProSelecta_detail::exception {
  using ProSelecta_detail::exception::exception;
};

template <typename Collection>
bool has_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "has_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_out_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::has_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev, PIDs);
}

bool has_out_part(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::has_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev, std::array{PID});
}

template <typename Collection>
bool has_exact_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs,
                        Collection const &counts) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "has_exact_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_exact_out_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::has_particles_exact<
      ProSelecta_detail::kUndecayedPhysical>(ev, PIDs, counts);
}

bool has_exact_out_part(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_exact<
      ProSelecta_detail::kUndecayedPhysical>(ev, std::array{PID}, {count});
}

template <typename Collection>
bool out_part_topology_matches(HepMC3::GenEvent const &ev,
                               Collection const &PIDs,
                               Collection const &counts) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "out_part_topology_matches: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
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

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return has_exact_out_part(ev, PIDs, counts) &&
         (ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
              ev, std::array<int, 0>{})
              .size() == std::accumulate(counts.begin(), counts.end(), 0));
}

template <typename Collection>
bool has_at_least_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs,
                           Collection const &counts) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "has_at_least_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
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

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical>(ev, PIDs, counts);
}

bool has_at_least_out_part(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical>(ev, std::array{PID}, {count});
}

template <typename Collection>
auto num_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {

  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "num_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("num_out_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ProSelecta_detail::broadcast_return<Collection, int>::type outs;

  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
            ev, std::array{PIDs[i]})
            .size();
  }
  return outs;
}

int num_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
               ev, std::array{PID})
        .size();
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
             ev, std::array<int, 0>{})
      .size();
}

template <typename Collection>
int num_out_part_except(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "num_out_part_except: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("num_out_part_except passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      Collection,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs)
      .size();
}

int num_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return num_out_part_except(ev, std::array{PID});
}

template <typename Collection>
auto all_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "all_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("all_out_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ProSelecta_detail::broadcast_return<
      Collection, std::vector<HepMC3::ConstGenParticlePtr>>::type outs;

  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
            ev, std::array{PIDs[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr>
all_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
        ev, std::array{PID});
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
      ev, std::array<int, 0>{});
}

template <typename Collection>
auto hm_out_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "hm_out_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("hm_out_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  typename ProSelecta_detail::broadcast_return<
      Collection, HepMC3::ConstGenParticlePtr>::type outs;

  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    outs.resize(PIDs.size());
  }

  for (size_t i = 0; i < PIDs.size(); ++i) {
    outs[i] = ps::part::highest(
        ps::p3mod,
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
            ev, std::array{PIDs[i]}));
  }
  return outs;
}

HepMC3::ConstGenParticlePtr hm_out_part(HepMC3::GenEvent const &ev, int PID) {
  return ps::part::highest(
      ps::p3mod,
      ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
          ev, std::array{PID}));
}

template <typename Collection>
auto all_out_part_except(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      Collection,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs);
}

auto all_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      std::array<int, 1>,
                                      ProSelecta_detail::kNotFromPDGList>(
      ev, std::array{PID});
}

template <typename Collection>
bool has_beam_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {

  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "has_beam_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_beam_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::particles<ProSelecta_detail::kBeam>(ev, PIDs)
      .size();
}

bool has_beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kBeam>(
        ev, std::array{PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam>(
             ev, std::array<int, 0>{})
      .size();
}

auto beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts = PID ? ProSelecta_detail::particles<ProSelecta_detail::kBeam>(
                         ev, std::array{PID})
                   : ProSelecta_detail::particles<ProSelecta_detail::kBeam>(
                         ev, std::array<int, 0>{});

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
auto beam_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "beam_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("beam_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  auto parts = ProSelecta_detail::particles<ProSelecta_detail::kBeam>(ev, PIDs);

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
bool has_target_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {

  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "has_target_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("has_target_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  return ProSelecta_detail::particles<ProSelecta_detail::kTarget>(ev, PIDs)
      .size();
}

bool has_target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kTarget>(
        ev, std::array{PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kTarget>(
             ev, std::array<int, 0>{})
      .size();
}

auto target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts = PID ? ProSelecta_detail::particles<ProSelecta_detail::kTarget>(
                         ev, std::array{PID})
                   : ProSelecta_detail::particles<ProSelecta_detail::kTarget>(
                         ev, std::array<int, 0>{});
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
auto target_part(HepMC3::GenEvent const &ev, Collection const &PIDs) {
  static_assert(ProSelecta_detail::is_zero_std_array<Collection>::value,
                "target_part: EmptyPIDList");
  if constexpr (ProSelecta_detail::is_std_vector_int<Collection>::value) {
    if (!PIDs.size()) {
      throw EmptyPIDList("target_part passed empty PID list");
    }
  }

  static_assert(
      ProSelecta_detail::is_std_vector_or_array_int<Collection>::value,
      "PIDs type must be a std::array<int,N> or std::vector<int>");

  auto parts =
      ProSelecta_detail::particles<ProSelecta_detail::kTarget>(ev, PIDs);

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

auto out_nuclear_parts(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::nuclear_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev);
}

int signal_process_id(HepMC3::GenEvent const &ev) {

  auto const &attr_names = ev.attribute_names();
  if (std::find(attr_names.begin(), attr_names.end(), "signal_process_id") ==
      attr_names.end()) {
    throw NoSignalProcessId("Event contains no signal_process_id attribute");
  }

  return ev.attribute<HepMC3::IntAttribute>("signal_process_id")->value();
}

} // namespace event
} // namespace ps
