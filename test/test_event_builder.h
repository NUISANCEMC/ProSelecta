#pragma once

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "TRandom.h"

#include <cmath>

double default_mass_GeV(int pid) {
  switch (pid) {
  case 2112:
  case 2212: {
    return .938;
  }
  case 1000060120: {
    return 12;
  }
  case 1000060110: {
    return 11;
  }
  case 12:
  case 14:
  case -12:
  case -14: {
    return 0;
  }
  case 13:
  case -13: {
    return .105;
  }
  case 11:
  case -11: {
    return 511E-6;
  }
  case 211:
  case -211: {
    return .140;
  }
  case 111: {
    return .135;
  }
  default: {
    return 0;
  }
  }
}

// Add particles with format:
// PID Status P3_GeV [theta_deg [mass] ]
// e.g. 2212 11 1
//   -- Proton, status 11 with 1 GeV of 3-mom in a random direction with
//   default mass
// e.g. 2212 11 1 20
//   -- Proton, status 11 with 1
// GeV of 3-mom 20 degrees from z with default mass
// e.g. 2212 11 1 20 .938
//   -- Proton, status 11 with 1 GeV of 3-mom 20 degrees from z with .938 GeV
//   of .938 GeV mass
// e.g. 2212 11 1 20 .938 45
//   -- Proton, status 11 with 1 GeV of 3-mom 20 degrees from z with .938 GeV
//   of .938 GeV mass and with an azimuthal angle of 45 degrees from x-dir
// e.g. 2212 11 1 20 - 45
//   -- Proton, status 11 with 1 GeV of 3-mom 20 degrees from z with the
//   default mass and with an azimuthal angle of 45 degrees from x-dir
std::shared_ptr<HepMC3::GenParticle> BuildPart(std::string const &pline) {

  std::vector<std::string> split;
  size_t ptr = 0;
  while (ptr != std::string::npos) {
    size_t ns = pline.find_first_of(' ', ptr);
    split.push_back(pline.substr(ptr, ns - ptr));

    if (ns == std::string::npos) {
      break;
    }

    ptr = ns + 1;
    if (ptr >= pline.length()) {
      ptr = std::string::npos;
    }
  }

  int pdg = std::stol(split[0]);
  int status = std::stol(split[1]);
  double p3mag = std::stod(split[2]) * 1000.0;

  double pol_rad = std::acos(gRandom->Uniform(-1, 1));
  if (split.size() >= 4) {
    pol_rad = std::stod(split[3]) * (M_PI / 180.0);
  }
  double mass = default_mass_GeV(pdg) * 1000.0;
  if ((split.size() >= 5) && (split[4] != "-")) {
    mass = std::stod(split[4]) * 1000.0;
  }
  double azim_rad = gRandom->Uniform(0, 2 * M_PI);
  if (split.size() >= 6) {
    azim_rad = std::stod(split[5]) * (M_PI / 180.0);
  }

  double x = p3mag * std::sin(pol_rad) * std::cos(azim_rad);
  double y = p3mag * std::sin(pol_rad) * std::sin(azim_rad);
  double z = p3mag * std::cos(pol_rad);
  double e = std::sqrt(std::pow(p3mag, 2) + std::pow(mass, 2));

  return std::make_shared<HepMC3::GenParticle>(HepMC3::FourVector{x, y, z, e},
                                               pdg, status);
}

HepMC3::GenEvent BuildEvent(std::vector<std::vector<std::string>> const &parts,
                            std::vector<int> const &vids = {}) {

  HepMC3::GenEvent evt(HepMC3::Units::MEV, HepMC3::Units::CM);

  for (size_t v = 0; v < (parts.size() - 1); ++v) {

    HepMC3::GenVertexPtr vertex = std::make_shared<HepMC3::GenVertex>();

    if (vids.size() > v) {
      vertex->set_status(vids[v]);
    }

    if (v == 0) {
      for (auto const &pline : parts[v]) {
        vertex->add_particle_in(BuildPart(pline));
      }
    } else {
      for (auto &part : evt.vertices().back()->particles_out()) {
        if (part->status() != 0) {
          vertex->add_particle_in(part);
        }
      }
    }

    for (auto const &pline : parts[v + 1]) {
      vertex->add_particle_out(BuildPart(pline));
    }

    evt.add_vertex(vertex);
  }

  return evt;
}