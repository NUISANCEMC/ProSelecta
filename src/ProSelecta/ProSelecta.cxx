#include "ProSelecta/ProSelecta.h"

#include "HepMC3/GenEvent.h"

#include "TInterpreter.h"

#include <cassert>
#include <filesystem>
#include <regex>
#include <stdexcept>

ProSelecta *ProSelecta::instance_ = nullptr;

std::function<std::tuple<bool, bool, bool>()> type_check_helper;
// use cling to check the return type of a named function declared to cling
bool returns_int(std::string const &symname,
                 TInterpreter::EErrorCode &cling_err) {
  gInterpreter->ProcessLine(
      (std::string(
           "ProSelecta_detail_FillFuncReturnTypeDeductions<decltype(std:"
           ":function{") +
       symname + "})::result_type>(0);")
          .c_str(),
      &cling_err);

  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    return false;
  }

  return std::get<0>(type_check_helper());
}
// use cling to check the return type of a named function declared to cling
bool returns_double(std::string const &symname,
                    TInterpreter::EErrorCode &cling_err) {
  gInterpreter->ProcessLine(
      (std::string(
           "ProSelecta_detail_FillFuncReturnTypeDeductions<decltype(std:"
           ":function{") +
       symname + "})::result_type>(0);")
          .c_str(),
      &cling_err);

  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    return false;
  }

  return std::get<1>(type_check_helper());
}

ProSelecta &ProSelecta::Get() {
  if (!instance_) {
    instance_ = new ProSelecta();

    gInterpreter->LoadText(R"(
static bool ProSelecta_detail_func_return_type_is_int = false;
static bool ProSelecta_detail_func_return_type_is_double = false;
static bool ProSelecta_detail_func_return_type_is_vect = false;

template <typename T> void ProSelecta_detail_FillFuncReturnTypeDeductions(
  std::enable_if_t<!std::is_same_v<T,void>,int>) {
  ProSelecta_detail_func_return_type_is_int = std::is_same_v<T, int>;
  ProSelecta_detail_func_return_type_is_double = std::is_same_v<T, double>;
  ProSelecta_detail_func_return_type_is_vect = 
    std::is_same_v<T, std::vector<double>>;
}

std::tuple<bool, bool, bool> 
ProSelecta_detail_GetFuncReturnTypeDeductions() {
  return {ProSelecta_detail_func_return_type_is_int, 
    ProSelecta_detail_func_return_type_is_double, 
    ProSelecta_detail_func_return_type_is_vect};
}

int ProSelecta_detail_test_int(){ return 1; }
std::vector<double> ProSelecta_detail_test_vector_double(){ return {1,}; }

      )");

    type_check_helper = VoidToFunctionPtr<std::tuple<bool, bool, bool> (*)()>(
        instance_->GetMangledNameWithPrototype(
            "ProSelecta_detail_GetFuncReturnTypeDeductions", ""));
    TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
    assert(returns_int("ProSelecta_detail_test_int", cling_err));
    if (cling_err != TInterpreter::EErrorCode::kNoError) {
      std::cerr << "ProSelecta self tests failed." << std::endl;
      abort();
    }
    assert(returns_double("ProSelecta_detail_test_vector_double", cling_err));
    if (cling_err != TInterpreter::EErrorCode::kNoError) {
      std::cerr << "ProSelecta self tests failed." << std::endl;
      abort();
    }
  }

  std::string paths = std::getenv("ProSelecta_INCLUDE_PATH");
  const std::regex ws_re(":");
  for (auto it =
           std::sregex_token_iterator(paths.begin(), paths.end(), ws_re, -1);
       it != std::sregex_token_iterator(); ++it) {
    std::filesystem::path path = std::string(*it);
    if (path.empty() || !std::filesystem::exists(path)) {
      continue;
    }
    instance_->AddIncludePath(path.native());
  }

  instance_->LoadText(R"(#include "ProSelecta/env/env.h")",
                      ProSelecta::Interpreter::kCling);
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
    std::cout
        << "\n\n!N.B. the constness of arguments is important, if you are "
           "sure a function like this should have been declared to "
           "cling, double check the constness."
        << std::endl;
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
    ss << "Function: " << fnname << " was not declared to cling interpreter."
       << std::endl;
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
    auto fsel = FindClingSym<ProSelecta_ftypes::sel_p>(
        fnname, "HepMC3::GenEvent const &");
    TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
    if (!returns_int(fnname, cling_err)) {
      std::stringstream ss("");
      ss << "Function: " << fnname
         << " was requested as a filter function, but it does not return an "
            "integer."
         << std::endl;
      throw std::runtime_error(ss.str());
    }
    return fsel;
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
    auto fsel = FindClingSym<ProSelecta_ftypes::pro_p>(
        fnname, "HepMC3::GenEvent const &");
    TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
    if (!returns_double(fnname, cling_err)) {
      std::stringstream ss("");
      ss << "Function: " << fnname
         << " was requested as a projection function, but it does not return "
            "a "
            "double."
         << std::endl;
      throw std::runtime_error(ss.str());
    }
    return fsel;
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
    auto fsel = FindClingSym<ProSelecta_ftypes::wgt_p>(
        fnname, "HepMC3::GenEvent const &");
    TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
    if (!returns_double(fnname, cling_err)) {
      std::stringstream ss("");
      ss << "Function: " << fnname
         << " was requested as a weight function, but it does not return a "
            "double."
         << std::endl;
      throw std::runtime_error(ss.str());
    }
    return fsel;
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}
