#include "ProSelecta/ProSelecta.h"

#include "HepMC3/GenEvent.h"

#include "TInterpreter.h"

ProSelecta *ProSelecta::instance_ = nullptr;

ProSelecta &ProSelecta::Get() {
  if (!instance_) {
    instance_ = new ProSelecta();
  }
  return *instance_;
}

ProSelecta::ProSelecta() {}

bool ProSelecta::LoadText(std::string const &txt) {
  return bool(gInterpreter->LoadText(txt.c_str()));
}
void ProSelecta::AddIncludePath(std::string const &path) {
  gInterpreter->AddIncludePath(path.c_str());
}

bool ProSelecta::LoadFile(std::string const &file_to_read) {
  return !bool(gInterpreter->LoadFile(file_to_read.c_str()));
}

void *ProSelecta::GetMangledNameWithPrototype(std::string const &fnname,
                                              std::string const &arglist) {
  std::string mname = gInterpreter
                          ->GetMangledNameWithPrototype(nullptr, fnname.c_str(),
                                                        arglist.c_str())
                          .Data();

  void *sym = gInterpreter->FindSym(fnname.c_str());
  void *msym = gInterpreter->FindSym(mname.c_str());
  if (!sym && !msym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    std::cout << "Searched for " << fnname << " and C++ mangled: " << fnname
              << "(" << arglist << ")" << std::endl;
    return nullptr;
  }

  return sym ? sym : msym;
}

ProSelecta_ftypes::filter
ProSelecta::GetFilterFunction(std::string const &fnname) {
  return FindSym<ProSelecta_ftypes::filter>(fnname, "HepMC3::GenEvent const &");
}

ProSelecta_ftypes::project
ProSelecta::GetProjectionFunction(std::string const &fnname) {
  return FindSym<ProSelecta_ftypes::project>(fnname,
                                             "HepMC3::GenEvent const &");
}

ProSelecta_ftypes::weight
ProSelecta::GetWeightFunction(std::string const &fnname) {
  return FindSym<ProSelecta_ftypes::weight>(fnname, "HepMC3::GenEvent const &");
}

bool ProSelecta::Filter(HepMC3::GenEvent const &event,
                        ProSelecta_ftypes::filter fn) {
  return fn(event);
}

std::vector<double>
ProSelecta::Project(HepMC3::GenEvent const &event,
                    std::vector<ProSelecta_ftypes::project> fns) {
  std::vector<double> projections;
  for (auto &fn : fns) {
    projections.push_back(fn(event));
  }
  return projections;
}

std::vector<double>
ProSelecta::Weight(HepMC3::GenEvent const &event,
                   std::vector<ProSelecta_ftypes::weight> fns) {
  std::vector<double> weights;
  for (auto &fn : fns) {
    weights.push_back(fn(event));
  }
  return weights;
}