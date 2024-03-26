#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ProSelecta/FuncTypes.h"

namespace ps {

class ProSelecta {
private:
  ProSelecta();
  static ProSelecta *instance_;

public:
  enum class Interpreter { kAuto, kCling };

  static ProSelecta &Get();

  Interpreter ResolveType(std::string const &fnname,
                          std::string const &arglist = "");

  bool LoadFile(std::string const &, Interpreter itype = Interpreter::kAuto);
  bool LoadText(std::string const &, Interpreter itype = Interpreter::kAuto);
  void AddIncludePath(std::string const &,
                      Interpreter itype = Interpreter::kAuto);

  SelectFunc GetSelectFunction(std::string const &,
                                  Interpreter itype = Interpreter::kAuto);
  ProjectionFunc GetProjectionFunction(std::string const &,
                                       Interpreter itype = Interpreter::kAuto);
  WeightFunc GetWeightFunction(std::string const &,
                               Interpreter itype = Interpreter::kAuto);
};

} // namespace ps
