#include "Units.h"
#include "MissingDatum.h"
#include "ProSelecta/ftypes.h"
#include "ProSelecta/ProSelecta.h"

namespace ps {

// Phone book of different weight functions
namespace weight {

  // Cling function custom grabber
  ProSelecta_ftypes::wgt get(std::string name){
    return ProSelecta::Get().GetFilterFunction(name);
  };

  // Testing function
  double half_weight(HepMC3::GenEvent const &){
    return 0.5;
  }
}
}




