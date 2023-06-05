#include "ProSelecta/ProSelecta.h"

#include "HepMC3/GenEvent.h"

#include "TInterpreter.h"

// Borrowed from Cling::utils
template <class T> T VoidToFunctionPtr(void *ptr) {
  union {
    T f;
    void *v;
  } tmp;
  tmp.v = ptr;
  return tmp.f;
}

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

ProSelecta_ftypes::analysis
ProSelecta::GetAnalysisFunction(std::string const &fnname) {
  std::cout << fnname << std::endl;
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No analysis named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<ProSelecta_ftypes::analysis>(sym);
}
ProSelecta_ftypes::filter
ProSelecta::GetFilterFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<ProSelecta_ftypes::filter>(sym);
}
ProSelecta_ftypes::project
ProSelecta::GetProjectionFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<ProSelecta_ftypes::project>(sym);
}
ProSelecta_ftypes::weight
ProSelecta::GetWeightFunction(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return VoidToFunctionPtr<ProSelecta_ftypes::weight>(sym);
}

void *ProSelecta::FindSym(std::string const &fnname) {
  void *sym = gInterpreter->FindSym(fnname.c_str());
  if (!sym) {
    std::cout << "No function named: " << fnname << " declared to TCling."
              << std::endl;
    return nullptr;
  }
  return sym;
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