#include "ProSelecta/ProSelecta_cling.h"
#include "ProSelecta/env.h"

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include "boost/dll/import.hpp"
#include "boost/function.hpp"

#include <string>

namespace py = pybind11;

int callplugin(std::string const &fname, std::string const &funcname) {
  auto plugin =
      boost::dll::import_alias<int(std::string const &, std::string const &)>(
          "./libdummyPlugin.so", "load_and_call_select");

  return plugin(fname, funcname);
}

namespace ps {
static std::map<std::string, boost::function<SelectFunc(std::string const &,
                                                        std::string const &)>>
    plugins;
} // namespace ps

ps::SelectFunc fetchfunc(std::string const &fname, std::string const &funcname) {

  std::string fid = fname + funcname;

  if (!ps::plugins.count(fid)) {
    ps::plugins[fid] = boost::dll::import_alias<ps::SelectFunc(
        std::string const &, std::string const &)>("./libdummyPlugin.so",
                                                   "load_and_return");
  }

  return ps::plugins[fid](fname, funcname);
}

PYBIND11_MODULE(pyProSelectaPlugin, m) {
  m.doc() = "ProSelecta implementation in python";

  m.def("callplugin", &callplugin);
  m.def("fetchfunc", &fetchfunc);
}
