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

bool HasOutPart(HepMC3::GenEvent const &ev, std::vector<int> const &PIDs) {
  if (!PIDs.size()) {
    std::stringstream ss;
    ss << "NumOutPartExcept: EmptyPIDList";
    throw EmptyPIDList(ss.str());
  }
  return ProSelecta_detail::has_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev, PIDs);
}

bool HasOutPart(HepMC3::GenEvent const &ev, int PID) {
  return ProSelecta_detail::has_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev, {PID});
}

bool HasAtLeastOutPart(HepMC3::GenEvent const &ev, std::vector<int> const &PIDs,
                       std::vector<int> const &counts) {
  if (!PIDs.size()) {
    std::stringstream ss;
    ss << "HasAtLeastOutPart: EmptyPIDList";
    throw EmptyPIDList(ss.str());
  }
  if (PIDs.size() != counts.size()) {
    std::stringstream ss;
    ss << "HasAtLeastOutPart: MismatchedPIDAndCountsListLength(" << PIDs.size()
       << ", " << counts.size() << ")";
    throw MismatchedPIDAndCountsListLength(ss.str());
  }
  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical>(ev, PIDs, counts);
}

bool HasAtLeastOutPart(HepMC3::GenEvent const &ev, int PID, int count) {
  return ProSelecta_detail::has_particles_atleast<
      ProSelecta_detail::kUndecayedPhysical>(ev, {PID}, {count});
}

template <class... Types>
auto NumOutPart(HepMC3::GenEvent const &ev, Types const &...PIDs) {

  static_assert(sizeof...(Types), "NumOutPart: EmptyPIDList");

  std::array<int, sizeof...(Types)> outs;
  std::array<int, sizeof...(Types)> pidarr{PIDs...};
  for (size_t i = 0; i < sizeof...(Types); ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
            ev, {pidarr[i]})
            .size();
  }
  return outs;
}

int NumOutPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
               ev, {PID})
        .size();
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(ev,
                                                                             {})
      .size();
}

template <class... Types>
int NumOutPartExcept(HepMC3::GenEvent const &ev, Types const &...PIDs) {
  static_assert(sizeof...(Types), "NumOutPartExcept: EmptyPIDList");

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kNotFromPDGList>(
             ev, {PIDs...})
      .size();
}

template <class... Types>
auto AllOutPart(HepMC3::GenEvent const &ev, Types const &...PIDs) {
  static_assert(sizeof...(Types), "OutPart: EmptyPIDList");

  std::array<std::vector<HepMC3::ConstGenParticlePtr>, sizeof...(Types)> outs;
  std::array<int, sizeof...(Types)> pidarr{PIDs...};

  for (size_t i = 0; i < sizeof...(Types); ++i) {
    outs[i] =
        ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
            ev, {pidarr[i]});
  }
  return outs;
}

std::vector<HepMC3::ConstGenParticlePtr> AllOutPart(HepMC3::GenEvent const &ev,
                                                    int PID = 0) {
  if (PID) {
    return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
        ev, {PID});
  }

  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical>(
      ev, {});
}

template <class... Types>
auto AllOutPartExcept(HepMC3::GenEvent const &ev, Types const &...PIDs) {
  return ProSelecta_detail::particles<ProSelecta_detail::kUndecayedPhysical,
                                      ProSelecta_detail::kNotFromPDGList>(
      ev, {PIDs...});
}

bool HasBeamPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kBeam>(ev,
                                                                      {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kBeam>(ev, {}).size();
}

auto BeamPart(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kBeam>(ev, {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kBeam>(ev, {});

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

bool HasTargetPart(HepMC3::GenEvent const &ev, int PID = 0) {
  if (PID) {
    return ProSelecta_detail::has_particles<ProSelecta_detail::kTarget>(ev,
                                                                        {PID});
  }
  return ProSelecta_detail::particles<ProSelecta_detail::kTarget>(ev, {})
      .size();
}

auto TargetPart(HepMC3::GenEvent const &ev, int PID = 0) {
  auto parts =
      PID ? ProSelecta_detail::particles<ProSelecta_detail::kTarget>(ev, {PID})
          : ProSelecta_detail::particles<ProSelecta_detail::kTarget>(ev, {});
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

auto OutNuclearParts(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::nuclear_particles<
      ProSelecta_detail::kUndecayedPhysical>(ev);
}

} // namespace event
} // namespace ps
