#pragma once

#include <cmath>

namespace ps {
namespace unit {

constexpr double const missing_data = 0xdeadbeef;

constexpr double const MeV = 1; // leading unit
constexpr double const GeV = 1000*MeV;
constexpr double const keV = 1E-3*MeV;
constexpr double const eV  = 1E-6*MeV;

constexpr double const c_light = 1; // leading unit

constexpr double const GeV2 = GeV*GeV;
constexpr double const MeV2 = MeV*MeV;
constexpr double const keV2 = keV*keV;
constexpr double const eV2 = eV*eV;

constexpr double const GeV_c2 = GeV/c_light/c_light;
constexpr double const MeV_c2 = MeV/c_light/c_light;
constexpr double const keV_c2 = keV/c_light/c_light;
constexpr double const eV_c2 = eV/c_light/c_light;

constexpr double const cm = 1; // leading unit
constexpr double const m = 100*cm;
constexpr double const mm = 1E-2*m;
constexpr double const um = 1E-6*m;
constexpr double const nm = 1E-9*m;
constexpr double const pm = 1E-12*m;
constexpr double const fm = 1E-15*m;

constexpr double const rad = 1;
constexpr double const deg = M_PI / 180.0;

} // namespace units
} // namespace ps
