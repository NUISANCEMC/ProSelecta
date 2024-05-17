#include "MissingDatum.h"
#include "Units.h"

#include "NuHepMC/ReaderUtils.hxx"

namespace ps {
namespace select {

std::function<int(HepMC3::GenEvent const &ev)>
process_id_exact(int procid_exact) {
  return [=](HepMC3::GenEvent const &ev) {
    return NuHepMC::ER3::ReadProcessID(ev) == procid_exact;
  };
}

std::function<int(HepMC3::GenEvent const &ev)>
process_id_between(int procid_min, int procid_max) {
  return [=](HepMC3::GenEvent const &ev) {
    auto proc_id = NuHepMC::ER3::ReadProcessID(ev);
    return (procid_min <= proc_id) && (proc_id < procid_max);
  };
}

} // namespace select
} // namespace ps
