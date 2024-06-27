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

  Interpreter resolve_type(std::string const &fnname,
                          std::string const &arglist = "");

  bool load_file(std::string const &, Interpreter itype = Interpreter::kCling);
  bool load_text(std::string const &, Interpreter itype = Interpreter::kCling);
  bool load_analysis(std::string const &, std::string const &,
                     Interpreter itype = Interpreter::kCling);

  void add_include_path(std::string const &,
                        Interpreter itype = Interpreter::kCling);

  SelectFunc get_select_func(std::string const &,
                             Interpreter itype = Interpreter::kCling);
  SelectsFunc get_selects_func(std::string const &,
                               Interpreter itype = Interpreter::kCling);
  ProjectionFunc get_projection_func(std::string const &,
                                     Interpreter itype = Interpreter::kCling);
  ProjectionsFunc get_projections_func(std::string const &,
                                       Interpreter itype = Interpreter::kCling);
  WeightFunc get_weight_func(std::string const &,
                             Interpreter itype = Interpreter::kCling);
};

} // namespace ps
