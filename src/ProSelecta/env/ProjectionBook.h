#include "Units.h"
#include "MissingDatum.h"
#include "ProSelecta/ftypes.h"
#include "ProSelecta/ProSelecta.h"

//#include "NuHepMC/EventUtils.hxx"
// Phone book of different projection functions

namespace ps {
namespace project {

  // Custom function parser
  ProSelecta_ftypes::pro get(std::string name){
    return ProSelecta::Get().GetProjectionFunction(name);
  };

  // enu : True Enu Estimate : units (HepMC3 default)
  double enu(HepMC3::GenEvent const &ev){
    auto bpart = ps::sel::Beam(ev,14);
    if(bpart) {
      return bpart->momentum().e();
    }
    return kMissingDatum<double>;
  }

}
}

