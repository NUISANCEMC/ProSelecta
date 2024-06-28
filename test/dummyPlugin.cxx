#include "HepMC3/GenEvent.h"

#include "ProSelecta/ProSelecta.h"

#include "boost/dll/alias.hpp"

namespace ps {
int load_and_call_select(std::string const &fname,
                         std::string const &funcname) {
  ps::ProSelecta::Get().load_file(fname);
  HepMC3::GenEvent evt;
  return ps::ProSelecta::Get().get_select_func(funcname)(evt);
}

SelectFunc load_and_return(std::string const &fname,
                           std::string const &funcname) {
  ps::ProSelecta::Get().load_file(fname);
  return ps::ProSelecta::Get().get_select_func(funcname);
}
} // namespace ps

BOOST_DLL_ALIAS(ps::load_and_call_select, load_and_call_select);
BOOST_DLL_ALIAS(ps::load_and_return, load_and_return);
