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
bool HasOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "HasOutPart: EmptyPIDList");

  return ProSelecta_detail::has_particles<ProSelecta_detail::kUndecayedPhysical,
                                          N>(ev, PIDs);
}

bool HasOutPart(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::has_particles<ProSelecta_detail::kUndecayedPhysical,
                                          1>(ev, {PID});
}

template <size_t N>
bool HasAtLeastOutPart(HepMC3::GenEvent const &ev,
                       std::array<int, N> const &PIDs,
                       std::array<int, N> const &counts) {
  static_assert(N > 0, "HasAtLeastOutPart: EmptyPIDList");

  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical, N>(ev, PIDs, counts);
}

bool HasAtLeastOutPart(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical, 1>(ev, {PID}, {count});
}

template <size_t N>
auto NumOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {

  static_assert(N > 0, "NumOutPart: EmptyPIDList");

  std::array<int, N> outs;

  for (size_t i = 0; i < N; ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1>(
            ev, {PIDs[i]})
            .size();
  }
  return outs;
}

int NumOutPart(HepMC3::GenEvent const &ev, int PID = 0) {
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
int NumOutPartExcept(HepMC3::GenEvent const &ev,
                     std::array<int, N> const &PIDs) {
  static_assert(N > 0, "NumOutPartExcept: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, N,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs)
      .size();
}

int NumOutPartExcept(HepMC3::GenEvent const &ev, int PID) {
  return NumOutPartExcept<1>(ev, {PID});
}

template <size_t N>
auto AllOutPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> outs;

  for (size_t i = 0; i < N; ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1>(
            ev, {PIDs[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr> AllOutPart(HepMC3::GenEvent const &ev,
                                                    int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                        1>(ev, {PID});
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 0>(
      ev, {});
}

template <size_t N>
auto AllOutPartExcept(HepMC3::GenEvent const &ev,
                      std::array<int, N> const &PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, N,
                                      ProSelecta_detail::kNotFromPDGList>(ev,
                                                                          PIDs);
}

auto AllOutPartExcept(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical, 1,
                                      ProSelecta_detail::kNotFromPDGList>(
      ev, {PID});
}

template <size_t N>
bool HasBeamPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {

  static_assert(N > 0, "HasBeamPart: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kBeam, N>(ev, PIDs)
      .size();
}

bool HasBeamPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kBeam, 1>(ev,
                                                                         {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam, 0>(ev, {})
      .size();
}

auto BeamPart(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kBeam, 1>(ev, {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kBeam, 0>(ev, {});

  if (!parts.size()) {
    std::stringstream ss;
    ss << "BeamPart(" << PID << "): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "BeamPart(" << PID << "): MoreThanOneBeamPart";
    throw MoreThanOneBeamPart(ss.str());
  }
  return parts.front();
}

template <size_t N>
auto BeamPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  auto parts =
      ProSelecta_detail::particles<ProSelecta_detail::kBeam, N>(ev, PIDs);

  if (!parts.size()) {
    std::stringstream ss;
    ss << "BeamPart({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "BeamPart({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): MoreThanOneBeamPart";
    throw MoreThanOneBeamPart(ss.str());
  }
  return parts.front();
}

template <size_t N>
bool HasTargetPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {

  static_assert(N > 0, "HasTargetPart: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kTarget, N>(ev, PIDs)
      .size();
}

bool HasTargetPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kTarget, 1>(
        ev, {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kTarget, 0>(ev, {})
      .size();
}

auto TargetPart(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kTarget, 1>(ev,
                                                                        {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kTarget, 0>(ev, {});
  if (!parts.size()) {
    std::stringstream ss;
    ss << "TargetPart(" << PID << "): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "TargetPart(" << PID << "): MoreThanOneTargetPart";
    throw MoreThanOneTargetPart(ss.str());
  }
  return parts.front();
}

template <size_t N>
auto TargetPart(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs) {
  static_assert(N > 0, "OutPart: EmptyPIDList");

  auto parts =
      ProSelecta_detail::particles<ProSelecta_detail::kTarget, N>(ev, PIDs);

  if (!parts.size()) {
    std::stringstream ss;
    ss << "TargetPart({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): NoMatchingParts";
    throw NoMatchingParts(ss.str());
  }
  if (parts.size() > 1) {
    std::stringstream ss;
    ss << "TargetPart({";
    for (auto PID : PIDs) {
      ss << PID << ", ";
    }
    ss << "}): MoreThanOneTargetPart";
    throw MoreThanOneTargetPart(ss.str());
  }
  return parts.front();
}

auto OutNuclearParts(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::nuclear_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev);
}

} // namespace event
} // namespace ps
