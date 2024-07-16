#pragma once

#include "HepMC3/FourVector.h"

#include <algorithm>
#include <numeric>

namespace ps {
namespace vect {

inline HepMC3::FourVector direction(HepMC3::FourVector v) {
  v.set_e(0);
  if (v.p3mod() > 0) {
    v /= v.p3mod();
  }
  return v;
}

inline double dot(HepMC3::FourVector const &a, HepMC3::FourVector const &b) {
  return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

inline HepMC3::FourVector cross(HepMC3::FourVector const &a,
                                HepMC3::FourVector const &b) {
  auto i = a.y() * b.z() - a.z() * b.y();
  auto j = a.z() * b.x() - a.x() * b.z();
  auto k = a.x() * b.y() - a.y() * b.x();

  return HepMC3::FourVector{i, j, k, 0};
}

inline double angle(HepMC3::FourVector const &v,
                    HepMC3::FourVector const &refv) {
  double ptot2 = v.length2() * refv.length2();
  if (ptot2 <= 0) {
    return 0.0;
  }
  return std::acos(std::clamp(dot(v, refv) / sqrt(ptot2), -1.0, 1.0));
}

inline HepMC3::FourVector transverse(HepMC3::FourVector v,
                                     HepMC3::FourVector dir) {
  dir = direction(dir);
  v.set_e(0);
  auto long_comp = dir * dot(v, dir);
  return v - long_comp;
}

inline HepMC3::FourVector rotate(HepMC3::FourVector const &v,
                                 HepMC3::FourVector axis, double theta) {
  // from https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
  axis = direction(axis);
  return v * std::cos(theta) + cross(axis, v) * std::sin(theta) +
         axis * dot(axis, v) * (1.0 - std::cos(theta));
}

inline HepMC3::FourVector boost_beta(HepMC3::FourVector const &fv) {
  return direction(fv) * (fv.p3mod() / fv.e());
}

inline HepMC3::FourVector boost(HepMC3::FourVector const &fv,
                                HepMC3::FourVector const &boost_beta) {

  HepMC3::FourVector vo;

  // Boost this Lorentz vector
  double bx = boost_beta.x();
  double by = boost_beta.y();
  double bz = boost_beta.z();

  double b2 = bx * bx + by * by + bz * bz;
  double gamma = 1.0 / sqrt(1.0 - b2);
  double bp = bx * fv.x() + by * fv.y() + bz * fv.z();
  double gamma2 = b2 > 0 ? (gamma - 1.0) / b2 : 0.0;

  vo.set_x(fv.x() + gamma2 * bp * bx + gamma * bx * fv.e());
  vo.set_y(fv.y() + gamma2 * bp * by + gamma * by * fv.e());
  vo.set_z(fv.z() + gamma2 * bp * bz + gamma * bz * fv.e());
  vo.set_e(gamma * (fv.e() + bp));

  return vo;
}

} // namespace vect
} // namespace ps