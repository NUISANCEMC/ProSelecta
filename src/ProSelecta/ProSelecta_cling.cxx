#include "ProSelecta/ProSelecta_cling.h"
#include "ProSelecta/ProSelecta.h"

#include "TInterpreter.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <regex>
#include <stdexcept>

namespace ps {
namespace cling {

// Borrowed from Cling::utils
template <typename T> static T VoidToFunctionPtr(void *ptr) {
  union {
    T f;
    void *v;
  } tmp;
  tmp.v = ptr;
  return tmp.f;
}

std::function<std::tuple<bool, bool, bool>()> type_check_helper;
// use cling to check the return type of a named function declared to cling

template <size_t N>
bool returns_impl(std::string const &symname,
                  TInterpreter::EErrorCode &cling_err) {
  gInterpreter->ProcessLine(
      (std::string(
           "ProSelecta_detail_FillFuncReturnTypeDeductions<decltype(std:"
           ":function{") +
       symname + "})::result_type>();")
          .c_str(),
      &cling_err);

  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    return false;
  }

  return std::get<N>(type_check_helper());
}

bool returns_int(std::string const &symname,
                 TInterpreter::EErrorCode &cling_err) {
  return returns_impl<0>(symname, cling_err);
}
bool returns_double(std::string const &symname,
                    TInterpreter::EErrorCode &cling_err) {
  return returns_impl<1>(symname, cling_err);
}

bool returns_vector_double(std::string const &symname,
                           TInterpreter::EErrorCode &cling_err) {
  return returns_impl<2>(symname, cling_err);
}

bool load_file(std::string const &file_to_read) {
  ps::cling::initialize_environment();
  return !bool(gInterpreter->LoadFile(file_to_read.c_str()));
}
bool load_text(std::string const &txt) {
  ps::cling::initialize_environment();
  return bool(gInterpreter->LoadText(txt.c_str()));
}
void add_include_path(std::string const &path) {
  gInterpreter->AddIncludePath(path.c_str());
}

bool func_is_defined(std::string const &fnname, std::string const &arglist) {

  std::string mname = gInterpreter
                          ->GetMangledNameWithPrototype(nullptr, fnname.c_str(),
                                                        arglist.c_str())
                          .Data();

  void *sym = gInterpreter->FindSym(fnname.c_str());
  void *msym = gInterpreter->FindSym(mname.c_str());

  return bool(sym) || bool(msym);
}

void *get_func_with_prototype(std::string const &fnname,
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

template <typename T>
T CastClingSymbolToFunction(std::string const &fnname,
                            std::string const &arglist = "") {
  return VoidToFunctionPtr<typename T::result_type (*)(
      HepMC3::GenEvent const &)>(get_func_with_prototype(fnname, arglist));
}

template <typename T, size_t N> T get_func_impl(std::string const &fnname) {
  ps::cling::initialize_environment();

  auto cling_func =
      CastClingSymbolToFunction<T>(fnname, "HepMC3::GenEvent const &");
  TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
  if (!returns_impl<N>(fnname, cling_err)) {
    std::stringstream ss("");
    ss << "Function: " << fnname
       << " was requested, but it does not return the right type." << std::endl;
    throw std::runtime_error(ss.str());
  }
  return cling_func;
}

SelectFunc get_select_func(std::string const &fnname) {
  return get_func_impl<SelectFunc, 0>(fnname);
}

ProjectionFunc get_projection_func(std::string const &fnname) {
  return get_func_impl<ProjectionFunc, 1>(fnname);
}

ProjectionsFunc get_projections_func(std::string const &fnname) {
  return get_func_impl<ProjectionsFunc, 2>(fnname);
}

WeightFunc get_weight_func(std::string const &fnname) {
  return get_func_impl<WeightFunc, 1>(fnname);
}

bool cling_env_initialized = false;
void initialize_environment() {
  if (cling_env_initialized) {
    return;
  }

  gInterpreter->LoadText(R"(
static bool ProSelecta_detail_func_return_type_is_int = false;
static bool ProSelecta_detail_func_return_type_is_double = false;
static bool ProSelecta_detail_func_return_type_is_vect = false;

template <typename T> void ProSelecta_detail_FillFuncReturnTypeDeductions() {
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
double ProSelecta_detail_test_double(){ return 1; }
std::vector<double> ProSelecta_detail_test_vector_double(){ return {1,}; }
)");

  type_check_helper = VoidToFunctionPtr<std::tuple<bool, bool, bool> (*)()>(
      get_func_with_prototype("ProSelecta_detail_GetFuncReturnTypeDeductions",
                              ""));
  TInterpreter::EErrorCode cling_err = TInterpreter::EErrorCode::kNoError;
  assert(returns_int("ProSelecta_detail_test_int", cling_err));
  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    std::cerr << "ProSelecta self tests failed." << std::endl;
    abort();
  }
  assert(returns_double("ProSelecta_detail_test_double", cling_err));
  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    std::cerr << "ProSelecta self tests failed." << std::endl;
    abort();
  }
  assert(
      returns_vector_double("ProSelecta_detail_test_vector_double", cling_err));
  if (cling_err != TInterpreter::EErrorCode::kNoError) {
    std::cerr << "ProSelecta self tests failed." << std::endl;
    abort();
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
    gInterpreter->AddIncludePath(path.native().c_str());
  }

  if (!gInterpreter->LoadText(R"(#include "ProSelecta/env/env.h")")) {
    std::cerr << "ProSelecta environment initialization failed." << std::endl;
    abort();
  }

  cling_env_initialized = true;
}
} // namespace cling
} // namespace ps
