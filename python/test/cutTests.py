from pyHepMC3 import HepMC3 as hm

from pyProSelecta import event, part, unit, pdg, p3mod, theta

from math import acos, cos, sin, sqrt, pi
import random

rest_masses_GeV = {
  2112: .938,
  2212: .938,
  1000060120: 12,
  1000060110: 11,
  12: 0,
  14: 0,
  -12: 0,
  -14:  0,
  13: .105,
  -13: .105,
  11: 511E-6,
  -11: 511E-6,
  211: .140,
  -211: .140,
  111: .135,
}

def default_mass_GeV(pid):
  if pid in rest_masses_GeV.keys():
    return rest_masses_GeV[pid]
  return 0

def BuildPart(pline):
  split = pline.split()

  pdg = int(split[0])
  status = int(split[1])
  p3mag = float(split[2]) * 1000.0

  pol_rad = acos(random.uniform(-1, 1))
  if len(split) >= 4:
    pol_rad = float(split[3]) * (pi / 180.0)
  
  mass = default_mass_GeV(pdg) * 1000.0
  if (len(split) >= 5) and (split[4] != "-"):
    mass = float(split[4]) * 1000.0
  
  azim_rad = random.uniform(0, 2 * pi)
  if len(split) >= 6:
    azim_rad = float(split[5]) * (pi / 180.0)
  

  x = p3mag * sin(pol_rad) * cos(azim_rad)
  y = p3mag * sin(pol_rad) * sin(azim_rad)
  z = p3mag * cos(pol_rad)
  e = sqrt(p3mag*p3mag + mass*mass)

  return hm.GenParticle(hm.FourVector(x, y, z, e),
                                               pdg, status)

def REQUIRE_FALSE(val):
  assert(~val)

def REQUIRE(val):
  assert(val)

protons = [ BuildPart(x) for x in ["2212 1 1.5 60", "2212 1 1 20 - 45", "2212 1 0.5 70"]]

REQUIRE_FALSE((p3mod < 0.5 * unit.GeV_c)(protons[-1]))

REQUIRE((p3mod < 1 * unit.GeV_c)(protons[-1]))

REQUIRE((p3mod > 0.25 * unit.GeV_c)(protons[-1]))

REQUIRE(((p3mod < 1 * unit.GeV_c) & (p3mod > 0.25 * unit.GeV_c))(protons[-1]))

REQUIRE_FALSE(((p3mod > 1 * unit.GeV_c) & (p3mod > 0.25 * unit.GeV_c))(protons[-1]))

REQUIRE_FALSE((theta < 70 * unit.deg)(protons[-1]))
REQUIRE((theta <= 70 * unit.deg)(protons[-1]))
REQUIRE((theta >= 70 * unit.deg)(protons[-1]))
REQUIRE_FALSE((theta > 70 * unit.deg)(protons[-1]))

REQUIRE_FALSE((theta > 80 * unit.deg)(protons[-1]))
REQUIRE((theta < 80 * unit.deg)(protons[-1]))

REQUIRE(((theta > 60 * unit.deg) & (theta < 75 * unit.deg))(protons[-1]))

REQUIRE_FALSE(((theta > 70 * unit.deg) & (theta < 75 * unit.deg))(protons[-1]))

REQUIRE(len(part.filter((p3mod <= 1 * unit.GeV_c), protons)) == 2)
REQUIRE(len(part.filter((p3mod < 1 * unit.GeV_c), protons)) == 1)
REQUIRE(len(part.filter((p3mod > 1 * unit.GeV_c), protons)) == 1)
REQUIRE(len(part.filter((p3mod >= 1 * unit.GeV_c), protons)) == 2)

REQUIRE(len(part.filter((p3mod >= 1 * unit.GeV_c) & (p3mod < 1.5 * unit.GeV_c), protons)) ==
        1)
REQUIRE(len(part.filter((p3mod > 1 * unit.GeV_c) & (p3mod < 1.5 * unit.GeV_c), protons)) ==
        0)
REQUIRE(len(part.filter((p3mod < 1.5 * unit.GeV_c) & (p3mod >= 1 * unit.GeV_c), protons)) ==
        1)
REQUIRE(len(part.filter((p3mod >= 1 * unit.GeV_c) & (p3mod < 2 * unit.GeV_c), protons)) ==
        2)

REQUIRE(len(part.filter((theta <= 70 * unit.deg), protons)) == 3)
REQUIRE(len(part.filter((theta < 70 * unit.deg), protons)) == 2)
REQUIRE(len(part.filter((theta < 60 * unit.deg), protons)) == 1)

REQUIRE(len(part.filter((theta > 45 * unit.deg) & (theta < 70 * unit.deg), protons)) ==
        1)
REQUIRE(len(part.filter((theta > 60 * unit.deg) & (theta < 70 * unit.deg), protons)) ==
        0)
REQUIRE(len(part.filter((theta < 70 * unit.deg) & (theta > 60 * unit.deg), protons)) ==
        0)

REQUIRE(len(part.filter((theta < 80 * unit.deg) && (theta > 65 * unit.deg) &&
                         (p3mod >= 0.75 * unit.GeV_c) && (p3mod < 2 * unit.GeV_c),
                     protons)) == 0);
REQUIRE(len(part.filter((p3mod >= 0.75 * unit.GeV_c) && (p3mod < 2 * unit.GeV_c) &&
                         (theta < 80 * unit.deg) && (theta > 65 * unit.deg),
                     protons)) == 0);

REQUIRE(len(part.filter((theta < 80 * unit.deg) && (theta >= 60 * unit.deg) &&
                         (p3mod >= 0.75 * unit.GeV_c) && (p3mod < 2 * unit.GeV_c),
                     protons)) == 1);
REQUIRE(len(part.filter((p3mod >= 0.75 * unit.GeV_c) && (p3mod < 2 * unit.GeV_c) &&
                         (theta < 80 * unit.deg) && (theta >= 60 * unit.deg),
                     protons)) == 1);

REQUIRE(len(part.filter((theta < 80 * unit.deg) && (theta > 65 * unit.deg) &&
                         (p3mod >= 0.4 * unit.GeV_c) && (p3mod < 1 * unit.GeV_c),
                     protons)) == 1);
REQUIRE(len(part.filter((p3mod >= 0.4 * unit.GeV_c) && (p3mod < 1 * unit.GeV_c) &&
                         (theta < 80 * unit.deg) && (theta > 65 * unit.deg),
                     protons)) == 1);