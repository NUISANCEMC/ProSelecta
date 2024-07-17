#pragma once

#include <cmath>

namespace ps {
namespace unit {

constexpr double const MeV = 1; // leading unit
constexpr double const GeV = 1000 * MeV;
constexpr double const keV = 1E-3 * MeV;
constexpr double const eV = 1E-6 * MeV;

constexpr double const c_light = 1; // leading unit

constexpr double const GeV2 = GeV * GeV;
constexpr double const MeV2 = MeV * MeV;
constexpr double const keV2 = keV * keV;
constexpr double const eV2 = eV * eV;

constexpr double const GeV_c = GeV / c_light;
constexpr double const MeV_c = MeV / c_light;
constexpr double const keV_c = keV / c_light;
constexpr double const eV_c = eV / c_light;

constexpr double const GeV_c2 = GeV_c / c_light;
constexpr double const MeV_c2 = MeV_c / c_light;
constexpr double const keV_c2 = keV_c / c_light;
constexpr double const eV_c2 = eV_c / c_light;

constexpr double const cm = 1; // leading unit
constexpr double const m = 100 * cm;
constexpr double const mm = 1E-2 * m;
constexpr double const um = 1E-6 * m;
constexpr double const nm = 1E-9 * m;
constexpr double const pm = 1E-12 * m;
constexpr double const fm = 1E-15 * m;

constexpr double const rad = 1;
constexpr double const deg = M_PI / 180.0;

} // namespace unit

constexpr auto operator""_MeV(long double MeV) { return MeV; }
constexpr auto operator""_GeV(long double MeV) { return MeV * ps::unit::GeV; }
constexpr auto operator""_keV(long double MeV) { return MeV * ps::unit::keV; }
constexpr auto operator""_eV(long double MeV) { return MeV * ps::unit::eV; }

constexpr auto operator""_MeV2(long double MeV2) { return MeV2; }
constexpr auto operator""_GeV2(long double MeV2) {
  return MeV2 * ps::unit::GeV2;
}
constexpr auto operator""_keV2(long double MeV2) {
  return MeV2 * ps::unit::keV2;
}
constexpr auto operator""_eV2(long double MeV2) { return MeV2 * ps::unit::eV2; }

constexpr auto operator""_MeV_c(long double MeV_c) { return MeV_c; }
constexpr auto operator""_GeV_c(long double MeV_c) {
  return MeV_c * ps::unit::GeV_c;
}
constexpr auto operator""_keV_c(long double MeV_c) {
  return MeV_c * ps::unit::keV_c;
}
constexpr auto operator""_eV_c(long double MeV_c) {
  return MeV_c * ps::unit::eV_c;
}

constexpr auto operator""_MeV_c2(long double MeV_c2) { return MeV_c2; }
constexpr auto operator""_GeV_c2(long double MeV_c2) {
  return MeV_c2 * ps::unit::GeV_c2;
}
constexpr auto operator""_keV_c2(long double MeV_c2) {
  return MeV_c2 * ps::unit::keV_c2;
}
constexpr auto operator""_eV_c2(long double MeV_c2) {
  return MeV_c2 * ps::unit::eV_c2;
}

constexpr auto operator""_rad(long double rad) { return rad; }
constexpr auto operator""_deg(long double rad) { return rad * ps::unit::deg; }

constexpr auto operator""_MeV(unsigned long long MeV) { return MeV; }
constexpr auto operator""_GeV(unsigned long long MeV) {
  return MeV * ps::unit::GeV;
}
constexpr auto operator""_keV(unsigned long long MeV) {
  return MeV * ps::unit::keV;
}
constexpr auto operator""_eV(unsigned long long MeV) {
  return MeV * ps::unit::eV;
}

constexpr auto operator""_MeV2(unsigned long long MeV2) { return MeV2; }
constexpr auto operator""_GeV2(unsigned long long MeV2) {
  return MeV2 * ps::unit::GeV2;
}
constexpr auto operator""_keV2(unsigned long long MeV2) {
  return MeV2 * ps::unit::keV2;
}
constexpr auto operator""_eV2(unsigned long long MeV2) {
  return MeV2 * ps::unit::eV2;
}

constexpr auto operator""_MeV_c(unsigned long long MeV_c) { return MeV_c; }
constexpr auto operator""_GeV_c(unsigned long long MeV_c) {
  return MeV_c * ps::unit::GeV_c;
}
constexpr auto operator""_keV_c(unsigned long long MeV_c) {
  return MeV_c * ps::unit::keV_c;
}
constexpr auto operator""_eV_c(unsigned long long MeV_c) {
  return MeV_c * ps::unit::eV_c;
}

constexpr auto operator""_MeV_c2(unsigned long long MeV_c2) { return MeV_c2; }
constexpr auto operator""_GeV_c2(unsigned long long MeV_c2) {
  return MeV_c2 * ps::unit::GeV_c2;
}
constexpr auto operator""_keV_c2(unsigned long long MeV_c2) {
  return MeV_c2 * ps::unit::keV_c2;
}
constexpr auto operator""_eV_c2(unsigned long long MeV_c2) {
  return MeV_c2 * ps::unit::eV_c2;
}

constexpr auto operator""_rad(unsigned long long rad) { return rad; }
constexpr auto operator""_deg(unsigned long long rad) {
  return rad * ps::unit::deg;
}

} // namespace ps