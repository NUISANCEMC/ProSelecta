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

template <size_t N>
bool has_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "has_out_part: EmptyPIDList");

  return ProSelecta_detail::has_particles<ProSelecta_detail::kUndecayedPhysical,
                                          N>(ev, PIDs);
}

bool has_out_part(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::has_particles<ProSelecta_detail::kUndecayedPhysical,
                                          1>(ev, {PID});
}

template <size_t N>
bool has_exact_out_part(HepMC3::GenEvent const &ev,
                        std::array<int, N> const &PIDs,
                        std::array<int, N> const &counts) {
  static_assert(N > 0, "has_exact_out_part: EmptyPIDList");

  return ProSelecta_detail::has_particles_exact<
      ProSelecta_detail::kUndecayedPhysical, N>(ev, PIDs, counts);
}

bool has_exact_out_part(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_exact<
      ProSelecta_detail::kUndecayedPhysical, 1>(ev, {PID}, {count});
}

template <size_t N>
bool out_part_topology_matches(HepMC3::GenEvent const &ev,
                               std::array<int, N> const &PIDs,
                               std::array<int, N> const &counts) {
  static_assert(N > 0, "out_part_topology_matches: EmptyPIDList");

  return has_exact_out_part<N>(ev, PIDs, counts) &&
         (ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                       0>(ev, {})
              .size() == std::accumulate(counts.begin(), counts.end(), 0));
}

template <size_t N>
bool has_at_least_out_part(HepMC3::GenEvent const &ev,
                           std::array<int, N> const &PIDs,
                           std::array<int, N> const &counts) {
  static_assert(N > 0, "has_at_least_out_part: EmptyPIDList");

  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical, N>(ev, PIDs, counts);
}

bool has_at_least_out_part(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical, 1>(ev, {PID}, {count});
}

template <size_t N>
auto num_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {

  static_assert(N > 0, "num_out_part: EmptyPIDList");

  std::array<int, N> outs;

  for (size_t i = 0; i < N; ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1>(
            ev, {PIDs[i]})
            .size();
  }
  return outs;
}

int num_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                        1>(ev, {PID})
        .size();
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 0>(
             ev, {})
      .size();
}

template <size_t N>
int num_out_part_except(HepMC3::GenEvent const &ev,
                        std::array<int, N> const &PIDs) {
  static_assert(N > 0, "num_out_part_except: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, N,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs)
      .size();
}

int num_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return num_out_part_except<1>(ev, {PID});
}

template <size_t N>
auto all_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> outs;

  for (size_t i = 0; i < N; ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1>(
            ev, {PIDs[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr>
all_out_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                        1>(ev, {PID});
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 0>(
      ev, {});
}

template <size_t N>
auto all_out_part_except(HepMC3::GenEvent const &ev,
                         std::array<int, N> const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, N,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs);
}

auto all_out_part_except(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1,
                                      ProSelecta_detail::kNotFromPDGList>(
      ev, {PID});
}

template <size_t N>
bool has_beam_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {

  static_assert(N > 0, "has_beam_part: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kBeam, N>(ev, PIDs)
      .size();
}

bool has_beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kBeam, 1>(ev,
                                                                         {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam, 0>(ev, {})
      .size();
}

auto beam_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kBeam, 1>(ev, {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kBeam, 0>(ev, {});

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

template <size_t N>
auto beam_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  auto parts =
      ProSelecta_detail::particles<ProSelecta_detail::kBeam, N>(ev, PIDs);

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

template <size_t N>
bool has_target_part(HepMC3::GenEvent const &ev,
                     std::array<int, N> const &PIDs) {

  static_assert(N > 0, "has_target_part: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kTarget, N>(ev, PIDs)
      .size();
}

bool has_target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kTarget, 1>(
        ev, {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kTarget, 0>(ev, {})
      .size();
}

auto target_part(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kTarget, 1>(ev,
                                                                        {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kTarget, 0>(ev, {});
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

template <size_t N>
auto target_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  auto parts =
      ProSelecta_detail::particles<ProSelecta_detail::kTarget, N>(ev, PIDs);

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

} // namespace event
} // namespace ps
