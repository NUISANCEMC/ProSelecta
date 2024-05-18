#include "ProSelecta/missing_datum.h"
#include "ProSelecta/unit.h"

namespace ps {
namespace func {
namespace project {

// enu : True Enu Estimate : units (HepMC3 default)
double enu(HepMC3::GenEvent const &ev) {
  if (!event::HasBeamPart(ev, 14)) {
    return kMissingDatum<double>;
  }
  return event::BeamPart(ev, 14)->momentum().e();
}

} // namespace project
} // namespace func
} // namespace ps
