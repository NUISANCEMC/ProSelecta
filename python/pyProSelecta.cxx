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

//   auto m_ps_select = m.def_submodule("select", "ProSelecta select interface");
//   m_ps_select.def("get", &ps::cling::get_select_func);
//   m_ps_select.def("get_vect", &ps::cling::get_selects_func);
//   m_ps_select.def("process_id_exact", &ps::eventect::process_id_exact);
//   m_ps_select.def("process_id_between", &ps::eventect::process_id_between);

//   auto m_ps_project =
//       m.def_submodule("project", "ProSelecta projection interface");
//   m_ps_project.def("get", &ps::cling::get_projection_func);
//   m_ps_project.def("get_vect", &ps::cling::get_projections_func);
//   m_ps_project.def("enu", &ps::project::enu);

//   auto m_ps_weight = m.def_submodule("weight", "ProSelecta weight interface");
//   m_ps_weight.def("get", &ps::cling::get_weight_func);
//   m_ps_weight.def("half_weight", &ps::weight::half_weight);

  // Selectors.h
  py::module sel = m.def_submodule("sel", "The Selector functions");
  sel.def("Beam", &ps::event::Beam,
          "Get a beam particle with a given PID from the event if it exists.");
  sel.def("BeamAny", &ps::event::BeamAny,
          "Get a beam particle with one of a list of PIDs from the event if it "
          "exists.");
  sel.def("Beams", &ps::event::Beams, "");
  sel.def("BeamsAny", &ps::event::BeamsAny, "");
  sel.def("Target", &ps::event::Target, "");
  sel.def("OutPartFirst", &ps::event::OutPartFirst, "");
  sel.def("PrimaryLeptonsForNuCC", &ps::event::PrimaryLeptonsForNuCC, "");
  sel.def("OutPartFirstAny", &ps::event::OutPartFirstAny, "");
  sel.def("OutPartHM", &ps::event::OutPartHM, "");
  sel.def("OutPartHMAny", &ps::event::OutPartHMAny, "");
  sel.def("OutParts", &ps::event::OutParts, "");
  sel.def("OutPartsAny", &ps::event::OutPartsAny, "");
  sel.def("OutPartsExcept", &ps::event::OutPartsExcept, "");
  sel.def("OutPartsExceptAny", &ps::event::OutPartsExceptAny, "");
  sel.def("FilterBy3Mom", &ps::event::FilterBy3Mom, "");
  sel.def("OutNuclearParts", &ps::event::OutNuclearParts, "");

  // Projectors.h
  py::module parts =
      m.def_submodule("part", "The parts Projector functions");
  parts.def("q0", &ps::part::q0, "");
  parts.def("q3", &ps::part::q3, "");
  parts.def("Q2", &ps::part::Q2, "");
  parts.def("CosTheta", &ps::part::CosTheta, "");
  parts.def("Theta", &ps::part::Theta, "");
  parts.def("W", &ps::part::W, "");
  parts.def("EPmiss", &ps::part::EPmiss, "");
  py::module event =
      m.def_submodule("event", "The event Projector functions");
  event.def("q0", &ps::event::q0, "");
  event.def("q3", &ps::event::q3, "");
  event.def("Q2Lep", &ps::event::Q2Lep, "");
  event.def("CosThetaLep", &ps::event::CosThetaLep, "");
  event.def("ThetaLep", &ps::event::ThetaLep, "");

  // Units.h
  py::module units = m.def_submodule("units", "Units constants");
  units.attr("GeV") = ps::unit::GeV;
  units.attr("MeV") =  ps::unit::MeV;
  units.attr("GeV2") =  ps::unit::GeV2;
  units.attr("MeV2") =  ps::unit::MeV2;
  units.attr("GeV_c2") =  ps::unit::GeV_c2;
  units.attr("MeV_c2") =  ps::unit::MeV_c2;
  units.attr("rad") =  ps::unit::rad;
  units.attr("deg") =  ps::unit::deg;

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
  pdg.attr("kChargedLeptons_matter") =
      ps::pdg::kChargedLeptons_matter;
  pdg.attr("kChargedLeptons_amatter") =
      ps::pdg::kChargedLeptons_amatter;
  pdg.attr("kNeutralLeptons") = ps::pdg::kNeutralLeptons;
  pdg.attr("kNeutralLeptons_matter") =
      ps::pdg::kNeutralLeptons_matter;
  pdg.attr("kNeutralLeptons_amatter") =
      ps::pdg::kNeutralLeptons_amatter;
  pdg.attr("kLeptons") = ps::pdg::kLeptons;
}
