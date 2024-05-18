#pragma once

#include "HepMC3/GenEvent.h"

namespace ps {
namespace vect {

// parts::q0(particle, particle) -> real
double dot(HepMC3::FourVector &a, HepMC3::FourVector &b) {
  return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

HepMC3::FourVector cross(HepMC3::FourVector &a, HepMC3::FourVector &b) {
  auto i = a.y() * b.z() - a.z() * b.y();
  auto j = a.z() * b.x() - a.x() * b.z();
  auto k = a.x() * b.y() - a.y() * b.x();

  return HepMC3::FourVector{i, j, k, 0};
}

HepMC3::FourVector boost(const HepMC3::FourVector v1,
                         const HepMC3::FourVector boost) {

  HepMC3::FourVector vo;

  // Boost this Lorentz vector
  double bx = boost.x();
  double by = boost.y();
  double bz = boost.z();

  double b2 = bx * bx + by * by + bz * bz;
  double gamma = 1.0 / sqrt(1.0 - b2);
  double bp = bx * v1.x() + by * v1.y() + bz * v1.z();
  double gamma2 = b2 > 0 ? (gamma - 1.0) / b2 : 0.0;

  vo.set_x(v1.x() + gamma2 * bp * bx + gamma * bx * v1.e());
  vo.set_y(v1.y() + gamma2 * bp * by + gamma * by * v1.e());
  vo.set_z(v1.z() + gamma2 * bp * bz + gamma * bz * v1.e());
  vo.set_e(gamma * (v1.e() + bp));

  return vo;
}

double angle(HepMC3::FourVector v1, HepMC3::FourVector q) {
  double ptot2 = v1.length2() * q.length2();
  if (ptot2 <= 0) {
    return 0.0;
  } else {
    double arg = dot(v1, q) / sqrt(ptot2);
    if (arg > 1.0)
      arg = 1.0;
    if (arg < -1.0)
      arg = -1.0;
    return acos(arg);
  }
}

HepMC3::FourVector rotate(HepMC3::FourVector p1, HepMC3::FourVector p2) {
  return HepMC3::FourVector(); // to be added
}

HepMC3::FourVector transverse(HepMC3::FourVector a, HepMC3::FourVector b) {
  a.set_e(0);
  auto long_comp = b * dot(a, b);
  return a - long_comp;
}

} // namespace vect
} // namespace ps