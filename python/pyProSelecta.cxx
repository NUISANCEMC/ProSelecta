#include "ProSelecta/ProSelecta_cling.h"

#include "ProSelecta/env.h"

#include "pybind11/functional.h"
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include <string>

namespace py = pybind11;

void pyProSelectaExtNuInit(py::module &);

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

  py::class_<ps::cuts>(m, "cuts")
      .def("__call__", &ps::cuts::operator(), py::arg("event"))
      .def("__and__", &ps::cuts::operator&&, py::arg("other"))
      .def("__invert__", &ps::cuts::operator!);

#define CUTABLE_BINDINGS(CN)                                                   \
  py::class_<ps::detail::CN>(m, #CN)                                           \
      .def(py::init<>())                                                       \
      .def("__call__",                                                         \
           py::overload_cast<HepMC3::ConstGenParticlePtr>(                     \
               &ps::detail::CN::operator(), py::const_),                       \
           py::arg("part"))                                                    \
      .def(                                                                    \
          "__le__",                                                            \
          [](ps::detail::CN const &self, double threshold) {                   \
            return self <= threshold;                                          \
          },                                                                   \
          py::arg("threshold"))                                                \
      .def(                                                                    \
          "__lt__",                                                            \
          [](ps::detail::CN const &self, double threshold) {                   \
            return self < threshold;                                           \
          },                                                                   \
          py::arg("threshold"))                                                \
      .def(                                                                    \
          "__ge__",                                                            \
          [](ps::detail::CN const &self, double threshold) {                   \
            return self >= threshold;                                          \
          },                                                                   \
          py::arg("threshold"))                                                \
      .def(                                                                    \
          "__gt__",                                                            \
          [](ps::detail::CN const &self, double threshold) {                   \
            return self > threshold;                                           \
          },                                                                   \
          py::arg("threshold"))

  CUTABLE_BINDINGS(p3mod);
  CUTABLE_BINDINGS(energy);
  CUTABLE_BINDINGS(kinetic_energy);

  auto theta_class = CUTABLE_BINDINGS(theta);
  theta_class.def("__call__",
                  py::overload_cast<HepMC3::FourVector const &>(
                      &ps::detail::theta::operator(), py::const_),
                  py::arg("refvec"));

  auto costheta_class = CUTABLE_BINDINGS(costheta);
  costheta_class.def("__call__",
                     py::overload_cast<HepMC3::FourVector const &>(
                         &ps::detail::costheta::operator(), py::const_),
                     py::arg("refvec"));

  py::class_<ps::detail::momentum>(m, "momentum")
      .def(py::init<>())
      .def("__call__",
           py::overload_cast<HepMC3::ConstGenParticlePtr>(
               &ps::detail::momentum::operator(), py::const_),
           py::arg("part"));

  m.attr("p3mod") = ps::p3mod;
  m.attr("energy") = ps::energy;
  m.attr("kinetic_energy") = ps::kinetic_energy;
  m.attr("theta") = ps::theta;
  m.attr("costheta") = ps::costheta;
  m.attr("momentum") = ps::momentum;

#define EVENT_INT_OR_VECTPID_BINDING(mod, EVENTFUNC)                           \
  mod.def(                                                                     \
         #EVENTFUNC,                                                           \
         [](HepMC3::GenEvent const &ev, int PID) {                             \
           return ps::event::EVENTFUNC(ev, PID);                               \
         },                                                                    \
         py::arg("event"), py::arg("PID") = 0)                                 \
      .def(                                                                    \
          #EVENTFUNC,                                                          \
          [](HepMC3::GenEvent const &ev, std::vector<int> const &PIDs) {       \
            return ps::event::EVENTFUNC(ev, PIDs);                             \
          },                                                                   \
          py::arg("event"), py::arg("PIDs"))

#define EVENT_INT_OR_VECTPID_flatten_BINDING(mod, EVENTFUNC)                   \
  mod.def(                                                                     \
         #EVENTFUNC,                                                           \
         [](HepMC3::GenEvent const &ev, int PID) {                             \
           return ps::event::EVENTFUNC(ev, PID);                               \
         },                                                                    \
         py::arg("event"), py::arg("PID") = 0)                                 \
      .def(                                                                    \
          #EVENTFUNC,                                                          \
          [](HepMC3::GenEvent const &ev, std::vector<int> const &PIDs,         \
             bool flatten) {                                                   \
            if (flatten) {                                                     \
              return py::cast(ps::event::EVENTFUNC(ev, PIDs, ps::flatten));    \
            } else {                                                           \
              return py::cast(ps::event::EVENTFUNC(ev, PIDs));                 \
            }                                                                  \
          },                                                                   \
          py::arg("event"), py::arg("PIDs"), py::kw_only(),                    \
          py::arg("flatten") = false)

  auto m_ps_event = m.def_submodule("event", "ProSelecta event module");
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, has_out_part);
  EVENT_INT_OR_VECTPID_flatten_BINDING(m_ps_event, num_out_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, num_out_part_except);
  EVENT_INT_OR_VECTPID_flatten_BINDING(m_ps_event, all_out_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, all_out_part_except);
  EVENT_INT_OR_VECTPID_flatten_BINDING(m_ps_event, hm_out_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, has_beam_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, beam_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, has_target_part);
  EVENT_INT_OR_VECTPID_BINDING(m_ps_event, target_part);
  m_ps_event
      .def(
          "out_nuclear_parts",
          [](HepMC3::GenEvent const &ev) {
            return ps::event::out_nuclear_parts(ev);
          },
          py::arg("event"))
      .def("signal_process_id", &ps::event::signal_process_id,
           py::arg("event"));

#define PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, PROJNAME)                          \
  .def(                                                                        \
      #PARTSFNAME,                                                             \
      [](ps::detail::PROJNAME const &proj,                                     \
         std::vector<HepMC3::ConstGenParticlePtr> const &parts) {              \
        return ps::part::PARTSFNAME(proj, parts);                              \
      },                                                                       \
      py::arg("projector"), py::arg("parts"))                                  \
      .def(                                                                    \
          #PARTSFNAME,                                                         \
          [](ps::detail::PROJNAME const &proj,                                 \
             std::vector<std::vector<HepMC3::ConstGenParticlePtr>> const       \
                 &part_groups,                                                 \
             bool flatten) {                                                   \
            if (flatten) {                                                     \
              return py::cast(                                                 \
                  ps::part::PARTSFNAME(proj, part_groups, ps::flatten));       \
            } else {                                                           \
              return py::cast(ps::part::PARTSFNAME(proj, part_groups));        \
            }                                                                  \
          },                                                                   \
          py::arg("projector"), py::arg("part_groups"), py::kw_only(),         \
          py::arg("flatten") = false)

#define PARTSFUNC_BINDINGS(mod, PARTSFNAME)                                    \
  mod PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, p3mod)                               \
      PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, energy)                              \
          PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, kinetic_energy)                  \
              PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, theta)                       \
                  PARTSFUNC_PROJ_BINDINGS(PARTSFNAME, costheta)

  auto m_ps_part = m.def_submodule("part", "ProSelecta part module");
  PARTSFUNC_BINDINGS(m_ps_part, sort_ascending);
  PARTSFUNC_BINDINGS(m_ps_part, highest);
  PARTSFUNC_BINDINGS(m_ps_part, lowest);
  PARTSFUNC_BINDINGS(m_ps_part, sum);
  m_ps_part
      .def(
          "sum",
          [](ps::detail::momentum const &proj,
             std::vector<HepMC3::ConstGenParticlePtr> const &parts) {
            return ps::part::sum(proj, parts);
          },
          py::arg("projector"), py::arg("parts"))
      .def(
          "sum",
          [](ps::detail::momentum const &proj,
             std::vector<std::vector<HepMC3::ConstGenParticlePtr>> const
                 &part_groups,
             bool flatten) {
            if (flatten) {
              return py::cast(ps::part::sum(proj, part_groups, ps::flatten));
            } else {
              return py::cast(ps::part::sum(proj, part_groups));
            }
          },
          py::arg("projector"), py::arg("part_groups"), py::kw_only(),
          py::arg("flatten") = false);
  m_ps_part
      .def(
          "filter",
          [](ps::cuts const &cut,
             std::vector<HepMC3::ConstGenParticlePtr> const &parts) {
            return ps::part::filter(cut, parts);
          },
          py::arg("cuts"), py::arg("parts"))
      .def(
          "filter",
          [](ps::cuts const &cut,
             std::vector<std::vector<HepMC3::ConstGenParticlePtr>> const
                 &part_groups,
             bool flatten) {
            if (flatten) {
              return py::cast(ps::part::filter(cut, part_groups, ps::flatten));
            } else {
              return py::cast(ps::part::filter(cut, part_groups));
            }
          },
          py::arg("cuts"), py::arg("part_groups"), py::kw_only(),
          py::arg("flatten") = false);

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

  auto m_ps_ext = m.def_submodule("ext", "ProSelecta extra module");
  pyProSelectaExtNuInit(m_ps_ext);
}
