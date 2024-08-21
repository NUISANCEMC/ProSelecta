#include "ProSelecta/env.h"

#include "ProSelecta/ext/nu/event_proj.h"
#include "ProSelecta/ext/nu/event_topo.h"

#include "ProSelecta/FuncTypes.h"

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include <string>

namespace py = pybind11;

void pyProSelectaExtNuInit(py::module &m) {

  m.def("enu_GeV", ps::ext::nu::enu_GeV, py::arg("event"))
      .def("plep_GeV", ps::ext::nu::plep_GeV, py::arg("event"))
      .def("thetalep_deg", ps::ext::nu::thetalep_deg, py::arg("event"))
      .def("Q2lep_GeV2", ps::ext::nu::Q2lep_GeV2, py::arg("event"))
      .def("q0lep_GeV", ps::ext::nu::q0lep_GeV, py::arg("event"))
      .def("q3lep_GeV", ps::ext::nu::q3lep_GeV, py::arg("event"))
      .def("hm_pprot_GeV", ps::ext::nu::hm_pprot_GeV, py::arg("event"))
      .def("hm_thetaprot_deg", ps::ext::nu::hm_thetaprot_deg, py::arg("event"))
      .def("hm_ppip_GeV", ps::ext::nu::hm_ppip_GeV, py::arg("event"))
      .def("hm_thetapip_deg", ps::ext::nu::hm_thetapip_deg, py::arg("event"))
      .def("hm_ppim_GeV", ps::ext::nu::hm_ppim_GeV, py::arg("event"))
      .def("hm_thetapim_deg", ps::ext::nu::hm_thetapim_deg, py::arg("event"))
      .def("hm_ppi0_GeV", ps::ext::nu::hm_ppi0_GeV, py::arg("event"))
      .def("hm_thetapi0_deg", ps::ext::nu::hm_thetapi0_deg, py::arg("event"))
      .def("isCC", ps::ext::nu::isCC, py::arg("event"))
      .def("isCC0Pi", ps::ext::nu::isCC0Pi, py::arg("event"))
      .def("isNC0Pi", ps::ext::nu::isNC0Pi, py::arg("event"))
      .def("isCC1Pi", ps::ext::nu::isCC1Pi, py::arg("event"))
      .def("isNC1Pi", ps::ext::nu::isNC1Pi, py::arg("event"))
      .def("isCCMultiPi", ps::ext::nu::isCCMultiPi, py::arg("event"))
      .def("isNCMultiPi", ps::ext::nu::isNCMultiPi, py::arg("event"))
      .def("final_state_topology", ps::ext::nu::final_state_topology,
           py::arg("event"))
      .def(
          "signal_process_id_filter",
          [](int sid) -> ps::SelectFunc {
            return [sid](HepMC3::GenEvent const &ev) {
              return ps::event::signal_process_id(ev) == sid;
            };
          },
          py::arg("signal_process_id"))
      .def(
          "signal_process_id_filter",
          [](int sid_low, int sid_high) -> ps::SelectFunc {
            return [sid_low, sid_high](HepMC3::GenEvent const &ev) {
              return !((ps::event::signal_process_id(ev) < sid_low) ||
                       (ps::event::signal_process_id(ev) > sid_high));
            };
          },
          py::arg("signal_process_id_min"), py::arg("signal_process_id_max"));
}
