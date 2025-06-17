#include "ProSelecta/env.h"

#include "test_event_builder.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>

using namespace Catch::Matchers;
using namespace ps::unit;

TEST_CASE("unit constants", "[ps::unit]") {

  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E-3 * ps::unit::GeV, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E3 * ps::unit::keV, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E6 * ps::unit::eV, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E-3 * ps::unit::GeV_c, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E3 * ps::unit::keV_c, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E6 * ps::unit::eV_c, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV,
               WithinAbs(1E-6 * ps::unit::GeV2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV,
               WithinAbs(1E6 * ps::unit::keV2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV,
               WithinAbs(1E12 * ps::unit::eV2, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E-3 * ps::unit::GeV_c2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E3 * ps::unit::keV_c2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E6 * ps::unit::eV_c2, 1E-8));

  REQUIRE_THAT(M_PI * ps::unit::rad, WithinAbs(180 * ps::unit::deg, 1E-8));
}

TEST_CASE("literal constants", "[ps::unit]") {

  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E-3_GeV, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E3_keV, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV, WithinAbs(1E6_eV, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E-3_GeV_c, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E3_keV_c, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / ps::unit::c_light,
               WithinAbs(1E6_eV_c, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV, WithinAbs(1E-6_GeV2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV, WithinAbs(1E6_keV2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV * ps::unit::MeV, WithinAbs(1E12_eV2, 1E-8));

  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E-3_GeV_c2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E3_keV_c2, 1E-8));
  REQUIRE_THAT(1 * ps::unit::MeV / (ps::unit::c_light * ps::unit::c_light),
               WithinAbs(1E6_eV_c2, 1E-8));

  REQUIRE_THAT(M_PI * ps::unit::rad, WithinAbs(180_deg, 1E-8));
}
