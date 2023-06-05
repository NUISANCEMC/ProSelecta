#pragma once

#include <string>
#include <vector>

#include "ProSelecta/ftypes.h"

class ProSelecta {
private:
  ProSelecta();
  static ProSelecta *instance_;

public:
  static ProSelecta &Get();

  bool LoadFile(std::string const &);
  bool LoadText(std::string const &txt);
  void AddIncludePath(std::string const &path);

  ProSelecta_ftypes::filter GetFilterFunction(std::string const &);
  ProSelecta_ftypes::project GetProjectionFunction(std::string const &);
  ProSelecta_ftypes::weight GetWeightFunction(std::string const &);
  void *FindSym(std::string const &);

  bool Filter(HepMC3::GenEvent const &, ProSelecta_ftypes::filter fn);
  std::vector<double> Project(HepMC3::GenEvent const &,
                              std::vector<ProSelecta_ftypes::project> fns);
  std::vector<double> Weight(HepMC3::GenEvent const &,
                             std::vector<ProSelecta_ftypes::weight> fns);
};
