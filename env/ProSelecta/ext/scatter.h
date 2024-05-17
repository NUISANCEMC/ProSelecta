#pragma once

#include "ProSelecta/details.h"

namespace ps {
namespace ext {
namespace scatter {

// IsCC(event) -> bool
bool IsCC(HepMC3::GenEvent const &) {
  return true;
}
// IsCOH(event) -> bool
bool IsCOH(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "IsCOH");
}
// Is1p1h(event) -> bool
bool Is1p1h(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "Is1p1h");
}
// Is2p2h(event) -> bool
bool Is2p2h(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "Is2p2h");
}
// IsSPP(event) -> bool
bool IsSPP(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "IsSPP");
}
// IsRES(event) -> bool
bool IsRES(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "IsRES");
}
// IsDIS(event) -> bool
bool IsDIS(HepMC3::GenEvent const &ev) {
  return ProSelecta_detail::IsChannel(ev, "IsDIS");
}

} // namespace scatter
} // namespace ext
} // namespace ps
