#include "ProSelecta/ProSelecta.h"

#include "HepMC3/GenEvent.h"

#include "TInterpreter.h"

#include <stdexcept>

ProSelecta *ProSelecta::instance_ = nullptr;

ProSelecta &ProSelecta::Get() {
  if (!instance_) {
    instance_ = new ProSelecta();
  }
  return *instance_;
}

ProSelecta::ProSelecta() {}

ProSelecta::Interpreter GuessInterpreter(std::string const &path) {
  size_t last_dot = path.find_last_of('.');

  if ((last_dot == std::string::npos) || (last_dot == 0)) {
    std::stringstream ss("");
    ss << "When trying to find extension of file: " << path
       << " passed to ProSelecta interpreter, failed to determine extension.";
    throw std::runtime_error(ss.str().c_str());
  }

  std::string ex = path.substr(last_dot + 1);

  return ProSelecta::Interpreter::kCling;
}

bool ProSelecta::LoadText(std::string const &txt,
                          ProSelecta::Interpreter itype) {
  switch (itype) {
  case Interpreter::kAuto: {
    throw std::runtime_error(
        "Cannot call ProSelecta::LoadText with Interpreter type kAuto. "
        "Explicitly specify the interpreter type.");
  }
  case Interpreter::kCling: {
    std::cout << "CALLING INTERPRETER LOAD TEXT" << std::endl;
    return bool(gInterpreter->LoadText(txt.c_str()));
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

bool ProSelecta::LoadFile(std::string const &file_to_read,
                          ProSelecta::Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = GuessInterpreter(file_to_read);
  }

  std::cout << "PROSELECTA LOADING CLING" << std::endl;

  switch (itype) {
  case Interpreter::kCling: {
    return !bool(gInterpreter->LoadFile(file_to_read.c_str()));
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

void ProSelecta::AddIncludePath(std::string const &path) {
  gInterpreter->AddIncludePath(path.c_str());
}

bool ProSelecta::ClingFunctionIsDefined(std::string const &fnname,
                                        std::string const &arglist) {
  std::string mname = gInterpreter
                          ->GetMangledNameWithPrototype(nullptr, fnname.c_str(),
                                                        arglist.c_str())
                          .Data();

  void *sym = gInterpreter->FindSym(fnname.c_str());
  void *msym = gInterpreter->FindSym(mname.c_str());

  return bool(sym) || bool(msym);
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

ProSelecta::Interpreter ProSelecta::ResolveType(std::string const &fnname,
                                                std::string const &) {
  bool cf = ClingFunctionIsDefined(fnname, "HepMC3::GenEvent const &");

  if (cf) {
    return Interpreter::kCling;
  } else {
    std::stringstream ss("");
    ss << "Function: " << fnname
       << " was declared to neither cling nor python interpreter." << std::endl;
    throw std::runtime_error(ss.str());
  }
}

ProSelecta_ftypes::sel ProSelecta::GetFilterFunction(std::string const &fnname,
                                                     Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = ResolveType(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return FindClingSym<ProSelecta_ftypes::sel_p>(fnname,
                                                  "HepMC3::GenEvent const &");
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

ProSelecta_ftypes::pro
ProSelecta::GetProjectionFunction(std::string const &fnname,
                                  Interpreter itype) {
  if (itype == Interpreter::kAuto) {
    itype = ResolveType(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return FindClingSym<ProSelecta_ftypes::pro_p>(fnname,
                                                  "HepMC3::GenEvent const &");
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

ProSelecta_ftypes::wgt ProSelecta::GetWeightFunction(std::string const &fnname,
                                                     Interpreter itype) {
  if (itype == Interpreter::kAuto) {
    itype = ResolveType(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return FindClingSym<ProSelecta_ftypes::wgt_p>(fnname,
                                                  "HepMC3::GenEvent const &");
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}
