#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>

using namespace Catch::Matchers;

using namespace ps;

TEST_CASE("direction", "[ps::vect]") {

  HepMC3::FourVector v{1, 1, 0, 0};
  auto vd = vect::direction(v);

  REQUIRE_THAT(v.p3mod(), WithinAbs(std::sqrt(2.0), 1E-8));

  REQUIRE_THAT(vd.x(), WithinAbs(1.0 / std::sqrt(2.0), 1E-8));
  REQUIRE_THAT(vd.y(), WithinAbs(1.0 / std::sqrt(2.0), 1E-8));
  REQUIRE_THAT(vd.p3mod(), WithinAbs(1.0, 1E-8));
}

TEST_CASE("dot", "[ps::vect]") {

  HepMC3::FourVector xd{1, 0, 0, 0};
  HepMC3::FourVector yd{0, 1, 0, 0};
  HepMC3::FourVector zd{0, 0, 1, 0};

  REQUIRE_THAT(vect::dot(xd, xd), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::dot(xd, yd), WithinAbs(0, 1E-8));
}

TEST_CASE("cross", "[ps::vect]") {

  HepMC3::FourVector xd{1, 0, 0, 0};
  HepMC3::FourVector yd{0, 1, 0, 0};
  HepMC3::FourVector zd{0, 0, 1, 0};

  REQUIRE_THAT(vect::cross(xd, xd).p3mod(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::cross(xd, yd).z(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::cross(xd, yd).p3mod(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::cross(xd, zd).y(), WithinAbs(-1, 1E-8));
  REQUIRE_THAT(vect::cross(xd, zd).p3mod(), WithinAbs(1, 1E-8));
}

TEST_CASE("angle", "[ps::vect]") {

  HepMC3::FourVector xd{1, 0, 0, 0};
  HepMC3::FourVector yd{0, 1, 0, 0};
  HepMC3::FourVector zd{0, 0, 1, 0};

  HepMC3::FourVector fwd{0, 1, 1, 0};

  REQUIRE_THAT(vect::angle(xd, zd), WithinAbs(90 * unit::deg, 1E-8));
  REQUIRE_THAT(vect::angle(yd, zd), WithinAbs(90 * unit::deg, 1E-8));
  REQUIRE_THAT(vect::angle(zd, zd), WithinAbs(0 * unit::deg, 1E-8));

  REQUIRE_THAT(vect::angle(fwd, zd), WithinAbs(45 * unit::deg, 1E-8));

  REQUIRE_THAT(vect::angle(zd, HepMC3::FourVector{0, 0, 0, 0}),
               WithinAbs(0 * unit::deg, 1E-8));
}

TEST_CASE("transverse", "[ps::vect]") {

  HepMC3::FourVector xd{1, 0, 0, 0};
  HepMC3::FourVector yd{0, 1, 0, 0};
  HepMC3::FourVector zd{0, 0, 1, 0};

  HepMC3::FourVector fwd{0, 1, 1, 0};

  REQUIRE_THAT(vect::transverse(xd, zd).x(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::transverse(xd, zd).z(), WithinAbs(0, 1E-8));

  REQUIRE_THAT(vect::transverse(yd, zd).x(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::transverse(yd, zd).y(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::transverse(xd, zd).z(), WithinAbs(0, 1E-8));

  REQUIRE_THAT(vect::transverse(fwd, zd).x(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::transverse(fwd, zd).y(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::transverse(fwd, zd).z(), WithinAbs(0, 1E-8));
}

TEST_CASE("rotate", "[ps::vect]") {

  HepMC3::FourVector xd{1, 0, 0, 0};
  HepMC3::FourVector yd{0, 1, 0, 0};
  HepMC3::FourVector zd{0, 0, 1, 0};

  HepMC3::FourVector fwd{1, 0, 1, 0};

  REQUIRE_THAT(vect::rotate(xd, zd, 90 * unit::deg).x(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::rotate(xd, zd, 90 * unit::deg).y(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::rotate(xd, zd, -90 * unit::deg).y(), WithinAbs(-1, 1E-8));
  REQUIRE_THAT(vect::rotate(xd, zd, 90 * unit::deg).z(), WithinAbs(0, 1E-8));

  REQUIRE_THAT(vect::rotate(fwd, zd, 90 * unit::deg).x(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::rotate(fwd, zd, 90 * unit::deg).y(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::rotate(fwd, zd, 90 * unit::deg).z(), WithinAbs(1, 1E-8));

  REQUIRE_THAT(vect::rotate(zd, fwd, 180 * unit::deg).x(), WithinAbs(1, 1E-8));
  REQUIRE_THAT(vect::rotate(zd, fwd, 180 * unit::deg).y(), WithinAbs(0, 1E-8));
  REQUIRE_THAT(vect::rotate(zd, fwd, 180 * unit::deg).z(), WithinAbs(0, 1E-8));
}