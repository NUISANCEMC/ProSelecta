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

  bool LoadFile(std::string const &, Interpreter itype = Interpreter::kCling);
  bool LoadText(std::string const &, Interpreter itype = Interpreter::kCling);
  bool LoadAnalysis(std::string const &, std::string const &, Interpreter itype = Interpreter::kCling);

  void AddIncludePath(std::string const &,
                      Interpreter itype = Interpreter::kCling);

  SelectFunc GetSelectFunction(std::string const &,
                               Interpreter itype = Interpreter::kCling);
  ProjectionFunc GetProjectionFunction(std::string const &,
                                       Interpreter itype = Interpreter::kCling);
  WeightFunc GetWeightFunction(std::string const &,
                               Interpreter itype = Interpreter::kCling);
};

} // namespace ps
