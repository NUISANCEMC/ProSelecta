#include "ProSelecta/ProSelecta.h"
#include "ProSelecta/ProSelecta_cling.h"

#include "HepMC3/GenEvent.h"

#include <stdexcept>

namespace ps {

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

bool ProSelecta::load_text(std::string const &txt,
                           ProSelecta::Interpreter itype) {
  switch (itype) {
  case Interpreter::kAuto: {
    throw std::runtime_error(
        "Cannot call ProSelecta::load_text with Interpreter type kAuto. "
        "Explicitly specify the interpreter type.");
  }
  case Interpreter::kCling: {
    return cling::load_text(txt);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

bool ProSelecta::load_file(std::string const &file_to_read,
                           ProSelecta::Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = GuessInterpreter(file_to_read);
  }
  switch (itype) {
  case Interpreter::kCling: {
    return cling::load_file(file_to_read);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

bool ProSelecta::load_analysis(std::string const &file_to_read,
                               std::string const &path,
                               ProSelecta::Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = GuessInterpreter(file_to_read);
  }
  switch (itype) {
  case Interpreter::kCling: {
    return cling::load_analysis(file_to_read, path);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

void ProSelecta::add_include_path(std::string const &path,
                                  ProSelecta::Interpreter itype) {
  switch (itype) {
  case Interpreter::kAuto: {
    throw std::runtime_error(
        "Cannot call ProSelecta::add_include_path with Interpreter type kAuto. "
        "Explicitly specify the interpreter type.");
  }
  case Interpreter::kCling: {
    cling::add_include_path(path);
    return;
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

ProSelecta::Interpreter ProSelecta::resolve_type(std::string const &fnname,
                                                 std::string const &) {
  bool cf = ps::cling::func_is_defined(fnname, "HepMC3::GenEvent const &");

  if (cf) {
    return Interpreter::kCling;
  } else {
    std::stringstream ss("");
    ss << "Function: " << fnname << " was not declared to cling interpreter."
       << std::endl;
    throw std::runtime_error(ss.str());
  }
}

SelectFunc ProSelecta::get_select_func(std::string const &fnname,
                                       Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = resolve_type(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return cling::get_select_func(fnname);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

SelectsFunc ProSelecta::get_selects_func(std::string const &fnname,
                                         Interpreter itype) {

  if (itype == Interpreter::kAuto) {
    itype = resolve_type(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return cling::get_selects_func(fnname);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

ProjectionFunc ProSelecta::get_projection_func(std::string const &fnname,
                                               Interpreter itype) {
  if (itype == Interpreter::kAuto) {
    itype = resolve_type(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return cling::get_projection_func(fnname);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

ProjectionsFunc ProSelecta::get_projections_func(std::string const &fnname,
                                                 Interpreter itype) {
  if (itype == Interpreter::kAuto) {
    itype = resolve_type(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return cling::get_projections_func(fnname);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

WeightFunc ProSelecta::get_weight_func(std::string const &fnname,
                                       Interpreter itype) {
  if (itype == Interpreter::kAuto) {
    itype = resolve_type(fnname, "HepMC3::GenEvent const &");
  }

  switch (itype) {
  case Interpreter::kCling: {
    return cling::get_weight_func(fnname);
  }
  default: {
    throw std::runtime_error("invalid interpreter type");
  }
  }
}

} // namespace ps
