#include "ProSelecta/missing_datum.h"
#include "ProSelecta/unit.h"

namespace ps {
namespace func {
namespace select {

std::function<int(HepMC3::GenEvent const &ev)>
process_id_exact(int procid_exact) {
  return [=](HepMC3::GenEvent const &ev) {
    return false;
    // return NuHepMC::ER3::ReadProcessID(ev) == procid_exact;
  };
}

std::function<int(HepMC3::GenEvent const &ev)>
process_id_between(int procid_min, int procid_max) {
  return [=](HepMC3::GenEvent const &ev) {
    return false;
    // auto proc_id = NuHepMC::ER3::ReadProcessID(ev);
    // return (procid_min <= proc_id) && (proc_id < procid_max);
  };
}
} // namespace select
} // namespace func
} // namespace ps
