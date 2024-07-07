#include "ProSelecta/ProSelecta_cling.h"
#include "ProSelecta/env.h"

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include <string>

namespace py = pybind11;

PYBIND11_MODULE(pyProSelecta, m) {
  m.doc() = "ProSelecta implementation in python";

  m.add_object("hm", py::module::import("pyHepMC3"));

  m.def("load_file", &ps::cling::load_file);
  m.def("load_text", &ps::cling::load_text);
  m.def("load_analysis", &ps::cling::load_analysis);
  m.def("add_include_path", &ps::cling::add_include_path);

  m.attr("kMissingDatum") = ps::kMissingDatum<double>;

  auto m_ps_select = m.def_submodule("select", "ProSelecta select interface");
  m_ps_select.def("get", &ps::cling::get_select_func);
  m_ps_select.def("get_vect", &ps::cling::get_selects_func);

  auto m_ps_project =
      m.def_submodule("project", "ProSelecta projection interface");
  m_ps_project.def("get", &ps::cling::get_projection_func);
  m_ps_project.def("get_vect", &ps::cling::get_projections_func);

  auto m_ps_weight = m.def_submodule("weight", "ProSelecta weight interface");
  m_ps_weight.def("get", &ps::cling::get_weight_func);

  auto m_ps_event = m.def_submodule("event", "ProSelecta event module");
  m_ps_event.def(
      "has_out_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::has_out_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "num_out_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::num_out_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "all_out_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::all_out_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "hm_out_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::hm_out_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "has_beam_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::has_beam_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "beam_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::beam_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "has_target_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::has_target_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "target_part",
      [](HepMC3::GenEvent const &ev, int PID) {
        return ps::event::target_part(ev, PID);
      },
      py::arg("event"), py::arg("PID") = 0);
  m_ps_event.def(
      "out_nuclear_parts",
      [](HepMC3::GenEvent const &ev) {
        return ps::event::out_nuclear_parts(ev);
      },
      py::arg("event"));
  m_ps_event.def(
      "signal_process_id",
      [](HepMC3::GenEvent const &ev) {
        return ps::event::signal_process_id(ev);
      },
      py::arg("event"));

  // Units.h
  py::module units = m.def_submodule("unit", "Units constants");
  units.attr("GeV") = ps::unit::GeV;
  units.attr("MeV") = ps::unit::MeV;
  units.attr("GeV2") = ps::unit::GeV2;
  units.attr("MeV2") = ps::unit::MeV2;
  units.attr("GeV_c2") = ps::unit::GeV_c2;
  units.attr("MeV_c2") = ps::unit::MeV_c2;
  units.attr("rad") = ps::unit::rad;
  units.attr("deg") = ps::unit::deg;

  // PDGCodes.h
  py::module pdg = m.def_submodule("pdg", "pdg constants");
  pdg.attr("kNuE") = ps::pdg::kNuE;
  pdg.attr("kANuE") = ps::pdg::kANuE;
  pdg.attr("kNuMu") = ps::pdg::kNuMu;
  pdg.attr("kANuMu") = ps::pdg::kANuMu;
  pdg.attr("kElectron") = ps::pdg::kElectron;
  pdg.attr("kAElectron") = ps::pdg::kAElectron;
  pdg.attr("kMuon") = ps::pdg::kMuon;
  pdg.attr("kAMuon") = ps::pdg::kAMuon;
  pdg.attr("kPiPlus") = ps::pdg::kPiPlus;
  pdg.attr("kPiZero") = ps::pdg::kPiZero;
  pdg.attr("kPiMinus") = ps::pdg::kPiMinus;
  pdg.attr("kProton") = ps::pdg::kProton;
  pdg.attr("kAProton") = ps::pdg::kAProton;
  pdg.attr("kNeutron") = ps::pdg::kNeutron;
  pdg.attr("kANeutron") = ps::pdg::kANeutron;
  pdg.attr("kGamma") = ps::pdg::kGamma;

  pdg.attr("kChargedLeptons") = ps::pdg::kChargedLeptons;
  pdg.attr("kChargedLeptons_matter") = ps::pdg::kChargedLeptons_matter;
  pdg.attr("kChargedLeptons_amatter") = ps::pdg::kChargedLeptons_amatter;
  pdg.attr("kNeutralLeptons") = ps::pdg::kNeutralLeptons;
  pdg.attr("kNeutralLeptons_matter") = ps::pdg::kNeutralLeptons_matter;
  pdg.attr("kNeutralLeptons_amatter") = ps::pdg::kNeutralLeptons_amatter;
  pdg.attr("kLeptons") = ps::pdg::kLeptons;
}
