#include "Units.h"
#include "MissingDatum.h"
#include "ProSelecta/ftypes.h"
#include "ProSelecta/ProSelecta.h"

//#include "NuHepMC/EventUtils.hxx"
#include "NuHepMC/ReaderUtils.hxx"

namespace ps {
namespace filter {

  // Returns a Cling visible custom function
  ProSelecta_ftypes::sel get(std::string name){
    return ProSelecta::Get().GetFilterFunction(name);
  };

  // Dedicated Filter Functions

  // PLACEHOLDER!
  // proc_id : Process ID from HepMC3
  int proc_id(HepMC3::GenEvent const &){
    return NuHepMC::ER3::ReadProcessID(ev);
  }

}
}




