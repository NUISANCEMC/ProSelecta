#pragma once

#include "ProSelecta/FuncTypes.h"

namespace ps {
namespace cling {

void initialize_environment();

bool load_file(std::string const &);
bool load_text(std::string const &);
void add_include_path(std::string const &);

bool func_is_defined(std::string const &fnname, std::string const &arglist);

SelectFunc get_select_func(std::string const &);
ProjectionFunc get_projection_func(std::string const &);
ProjectionsFunc get_projections_func(std::string const &);
WeightFunc get_weight_func(std::string const &);

} // namespace cling
} // namespace ps
