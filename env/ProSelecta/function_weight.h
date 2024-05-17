#include "ProSelecta/missing_datum.h"
#include "ProSelecta/unit.h"

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




