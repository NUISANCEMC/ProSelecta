#include "ProSelecta/MissingDatum.h"
#include "ProSelecta/Units.h"

namespace ps {

// Phone book of different weight functions
namespace func {
namespace weight {

  // Testing function
  double half_weight(HepMC3::GenEvent const &){
    return 0.5;
  }
}
}




