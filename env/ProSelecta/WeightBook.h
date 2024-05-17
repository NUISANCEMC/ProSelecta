#include "Units.h"
#include "MissingDatum.h"

namespace ps {

// Phone book of different weight functions
namespace weight {

  // Testing function
  double half_weight(HepMC3::GenEvent const &){
    return 0.5;
  }
}
}




