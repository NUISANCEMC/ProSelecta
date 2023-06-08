#pragma once

#include "HepMC3/GenEvent.h"

#include <string>

namespace ProSelecta_detail {

bool IsChannel(HepMC3::GenEvent const &ev, std::string const &chan_attr) {
  auto attr = ev.attribute<HepMC3::BoolAttribute>(chan_attr);
  return attr && attr->value();
}

} // namespace ProSelecta_detail