#include "MissingDatum.h"
#include "Units.h"

namespace ps {
namespace project {

// enu : True Enu Estimate : units (HepMC3 default)
double enu(HepMC3::GenEvent const &ev) {
  auto bpart = ps::sel::Beam(ev, 14);
  if (bpart) {
    return bpart->momentum().e();
  }
  return kMissingDatum<double>;
}

} // namespace project
} // namespace ps
