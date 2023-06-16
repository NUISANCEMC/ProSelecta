from pyHepMC3 import HepMC3 as hm
import pyProSelecta as ps

evt = hm.GenEvent(hm.Units.GEV, hm.Units.MM)

v1 = hm.GenVertex()
evt.add_vertex(v1)

pi1 = hm.GenParticle(hm.FourVector(1,2,3,0), 14, 4)
pi2 = hm.GenParticle(hm.FourVector(1,2,3,0), 2112, 11)
po1 = hm.GenParticle(hm.FourVector(1,2,3,0), 13, 3)
po2 = hm.GenParticle(hm.FourVector(1,2,3,0), 2212, 3)

v1.add_particle_in(pi1)
v1.add_particle_in(pi2)
v1.add_particle_out(po1)
v1.add_particle_out(po2)

print("Listing:%s" % hm.Print.line(ps.GetBeam(evt, 14)))
if (ps.GetBeam(evt, -14) == None):
  print("Im fucking nothing mate")
if not ps.GetBeam(evt, -14):
  print("no particle here")
if ps.GetBeam(evt, 14):
  print("found this one though")
print("Listing:%s" % hm.Print.line(ps.GetBeam(evt, -14)))