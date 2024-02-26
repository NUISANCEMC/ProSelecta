#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include "ProSelecta/ProSelecta.h"
#include "ProSelecta/env/env.h"
#include "ProSelecta/ftypes.h"

#include <string>

namespace py = pybind11;

auto GetProjectionFunc(std::string key) {
  auto f = ProSelecta::Get().GetProjectionFunction(
      key, ProSelecta::Interpreter::kCling);
  if (!f) {
    std::cerr << "[pyProSelecta]: Failed to find Projections function: " << key
              << std::endl;
    abort();
  }
  return f;
}

auto GetFilterFunc(std::string key) {
  auto f =
      ProSelecta::Get().GetFilterFunction(key, ProSelecta::Interpreter::kCling);
  if (!f) {
    std::cerr << "[pyProSelecta]: Failed to find Filter function: " << key
              << std::endl;
    abort();
  }
  return f;
}

PYBIND11_MODULE(pyProSelecta, m) {
  m.doc() = "ProSelecta implementation in python";

  m.add_object("hm",py::module::import("pyHepMC3"));

  m.def("load_file", [](std::string analysis) {
    return ProSelecta::Get().LoadFile(analysis.c_str(),
                                      ProSelecta::Interpreter::kCling);
  });

  m.def("load_text", [](std::string analysis) {
    return ProSelecta::Get().LoadText(analysis.c_str(),
                                      ProSelecta::Interpreter::kCling);
  });

  // Heretic!
  m.def("create_filter", [](std::string filter, std::string analysis) {
    std::string filter_analysis =
        "int " + filter + "(HepMC3::GenEvent const &ev) {";
    filter_analysis += analysis;
    filter_analysis += "}";
    ProSelecta::Get().LoadText(filter_analysis.c_str(),
                               ProSelecta::Interpreter::kCling);
  });

  m.def("create_projection", [](std::string projection, std::string analysis) {
    std::string projection_analysis =
        "double " + projection + "(HepMC3::GenEvent const &ev) {";
    projection_analysis += analysis;
    projection_analysis += "}";
    ProSelecta::Get().LoadText(projection_analysis.c_str(),
                               ProSelecta::Interpreter::kCling);
  });

  m.def("add_include_path", [](std::string analysis) {
    ProSelecta::Get().AddIncludePath(analysis.c_str());
  });

  auto m_ps_filter = m.def_submodule("filter", "ProSelecta filter interface");
  m_ps_filter.def("__getattr__", &GetFilterFunc);

  auto m_ps_project =
      m.def_submodule("project", "ProSelecta projection interface");
  m_ps_project.def("__getattr__", &GetProjectionFunc);

  // Selectors.h
  py::module sel = m.def_submodule("sel", "The Selector functions");
  sel.def("Beam", &ps::sel::Beam,
          "Get a beam particle with a given PID from the event if it exists.");
  sel.def("BeamAny", &ps::sel::BeamAny,
          "Get a beam particle with one of a list of PIDs from the event if it "
          "exists.");
  sel.def("Beams", &ps::sel::Beams, "");
  sel.def("BeamsAny", &ps::sel::BeamsAny, "");
  sel.def("Target", &ps::sel::Target, "");
  sel.def("OutPartFirst", &ps::sel::OutPartFirst, "");
  sel.def("OutPartFirstAny", &ps::sel::OutPartFirstAny, "");
  sel.def("OutPartHM", &ps::sel::OutPartHM, "");
  sel.def("OutPartHMAny", &ps::sel::OutPartHMAny, "");
  sel.def("OutParts", &ps::sel::OutParts, "");
  sel.def("OutPartsAny", &ps::sel::OutPartsAny, "");
  sel.def("OutPartsExcept", &ps::sel::OutPartsExcept, "");
  sel.def("OutPartsExceptAny", &ps::sel::OutPartsExceptAny, "");

  // Projectors.h
  py::module proj = m.def_submodule("proj", "The Projector functions");
  py::module parts =
      proj.def_submodule("parts", "The parts Projector functions");
  parts.def("q0", &ps::proj::parts::q0, "");
  parts.def("q3", &ps::proj::parts::q3, "");
  parts.def("Q2", &ps::proj::parts::Q2, "");
  parts.def("CosTheta", &ps::proj::parts::CosTheta, "");
  parts.def("Theta", &ps::proj::parts::Theta, "");
  parts.def("W", &ps::proj::parts::W, "");
  parts.def("EPmiss", &ps::proj::parts::EPmiss, "");
  py::module event =
      proj.def_submodule("event", "The event Projector functions");
  event.def("q0", &ps::proj::event::q0, "");
  event.def("q3", &ps::proj::event::q3, "");
  event.def("Q2Lep", &ps::proj::event::Q2Lep, "");
  event.def("CosThetaLep", &ps::proj::event::CosThetaLep, "");
  event.def("ThetaLep", &ps::proj::event::ThetaLep, "");

  // Units.h
  py::module units = m.def_submodule("units", "Units constants");
  units.attr("GeV") = ps::GeV;
  units.attr("MeV") = ps::MeV;
  units.attr("GeV2") = ps::GeV2;
  units.attr("MeV2") = ps::MeV2;
  units.attr("GeV_c2") = ps::GeV_c2;
  units.attr("MeV_c2") = ps::MeV_c2;
  units.attr("rad") = ps::rad;
  units.attr("deg") = ps::deg;

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

  py::module pdg_groups = pdg.def_submodule("groups", "pdg constants");
  pdg_groups.attr("kChargedLeptons") = ps::pdg::groups::kChargedLeptons;
  pdg_groups.attr("kChargedLeptons_matter") =
      ps::pdg::groups::kChargedLeptons_matter;
  pdg_groups.attr("kChargedLeptons_amatter") =
      ps::pdg::groups::kChargedLeptons_amatter;
  pdg_groups.attr("kNeutralLeptons") = ps::pdg::groups::kNeutralLeptons;
  pdg_groups.attr("kNeutralLeptons_matter") =
      ps::pdg::groups::kNeutralLeptons_matter;
  pdg_groups.attr("kNeutralLeptons_amatter") =
      ps::pdg::groups::kNeutralLeptons_amatter;
  pdg_groups.attr("kLeptons") = ps::pdg::groups::kLeptons;
}
