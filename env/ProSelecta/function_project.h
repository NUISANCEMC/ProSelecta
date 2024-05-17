#include "ProSelecta/missing_datum.h"
#include "ProSelecta/unit.h"

namespace ps {
namespace func{
namespace project {

// enu : True Enu Estimate : units (HepMC3 default)
double enu(HepMC3::GenEvent const &ev) {
  auto bpart = ps::event::Beam(ev, 14);
  if (bpart) {
    return bpart->momentum().e();
  }
  return kMissingDatum<double>;
}
} // namespace project
} // namespace func
} // namespace ps
