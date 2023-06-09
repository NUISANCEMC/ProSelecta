#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ProSelecta/ftypes.h"

namespace pybind11 {
class scoped_interpreter;
}

class ProSelecta {
private:
  ProSelecta();
  static ProSelecta *instance_;

  std::unique_ptr<pybind11::scoped_interpreter> pyint_guard;

  void *GetMangledNameWithPrototype(std::string const &fnname,
                                    std::string const &arglist);

  // Borrowed from Cling::utils
  template <typename T> T VoidToFunctionPtr(void *ptr) {
    union {
      T f;
      void *v;
    } tmp;
    tmp.v = ptr;
    return tmp.f;
  }

public:
  enum class Interpreter { kAuto, kCling, kPython };

  static ProSelecta &Get();

  bool LoadFile(std::string const &, Interpreter itype = Interpreter::kAuto);
  bool LoadText(std::string const &, Interpreter itype = Interpreter::kAuto);
  void AddIncludePath(std::string const &);

  bool PythonFunctionIsDefined(std::string const &fnname);
  bool ClingFunctionIsDefined(std::string const &fnname,
                              std::string const &arglist = "");

  Interpreter ResolveType(std::string const &fnname,
                          std::string const &arglist = "");

  template <typename T>
  std::function<std::remove_pointer_t<T>>
  FindClingSym(std::string const &fnname, std::string const &arglist = "") {
    return VoidToFunctionPtr<T>(GetMangledNameWithPrototype(fnname, arglist));
  }

  ProSelecta_ftypes::sel
  GetFilterFunction(std::string const &,
                    Interpreter itype = Interpreter::kAuto);
  ProSelecta_ftypes::pro
  GetProjectionFunction(std::string const &,
                        Interpreter itype = Interpreter::kAuto);
  ProSelecta_ftypes::wgt
  GetWeightFunction(std::string const &,
                    Interpreter itype = Interpreter::kAuto);
};
