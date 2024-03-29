#pragma once

#include "PDGCodes.h"
#include "Selectors.h"

namespace ps {

namespace proj {
namespace parts {

// parts::q0(particle, particle) -> real
double q0(HepMC3::ConstGenParticlePtr pin, HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return 0xdeadbeef;
  }

  return (pin->momentum() - pout->momentum()).e();
}

// parts::q3(particle, particle) -> real
double q3(HepMC3::ConstGenParticlePtr pin, HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return 0xdeadbeef;
  }

  return (pin->momentum() - pout->momentum()).p3mod();
}

// parts::Q2(particle, particle) -> real
double Q2(HepMC3::ConstGenParticlePtr pin,
             HepMC3::ConstGenParticlePtr pout) {
  if (!pin || !pout) {
    return 0xdeadbeef;
  }

  return -(pin->momentum() - pout->momentum()).m2();
}

// parts::CosTheta(particle, particle) -> real
double CosTheta(HepMC3::ConstGenParticlePtr p1,
                HepMC3::ConstGenParticlePtr p2) {
  if (!p1 || !p2) {
    return 0xdeadbeef;
  }

  auto const &p1mom = p1->momentum();
  auto const &p2mom = p2->momentum();

  double dotp =
      p1mom.x() * p2mom.x() + p1mom.y() * p2mom.y() + p1mom.z() * p2mom.z();

  return dotp / (p1mom.p3mod() * p2mom.p3mod());
}

// parts::Theta(particle, particle) -> real
double Theta(HepMC3::ConstGenParticlePtr p1, HepMC3::ConstGenParticlePtr p2) {
  if (!p1 || !p2) {
    return 0xdeadbeef;
  }

  return std::acos(CosTheta(p1, p2));
}

// parts::W(list<particles>) -> real
double W(std::vector<HepMC3::ConstGenParticlePtr> parts) {
  HepMC3::FourVector fv;

  for (auto &p : parts) {
    if (!p) {
      return 0xdeadbeef;
    }
    fv += p->momentum();
  }

  return fv.m();
}

// parts::EPmiss(list<particles>) -> 4vec
HepMC3::FourVector EPmiss(std::vector<HepMC3::ConstGenParticlePtr> parts_in,
                          std::vector<HepMC3::ConstGenParticlePtr> parts_out) {
  HepMC3::FourVector fv_in, fv_out;

  for (auto &p : parts_in) {
    if (!p) {
      return HepMC3::FourVector{0xdeadbeef, 0, 0, 0};
    }
    fv_in += p->momentum();
  }

  for (auto &p : parts_out) {
    if (!p) {
      return HepMC3::FourVector{0xdeadbeef, 0, 0, 0};
    }
    fv_out += p->momentum();
  }

  return fv_in - fv_out;
}

// parts::Pt(list<particles>) -> real
double Pt(std::vector<HepMC3::ConstGenParticlePtr> parts) {
  HepMC3::FourVector fv_tot;

  for (auto &p : parts) {
    if (!p) {
      return 0xdeadbeef;
    }
    fv_tot += p->momentum();
  }

  return fv_tot.perp();
}

} // namespace parts

namespace event {

// event::q0(event) -> real
double q0(HepMC3::GenEvent const &ev) {

  auto pin = ps::sel::BeamAny(ev, pdg::groups::kNeutralLeptons);

  if (!pin) {
    return 0xdeadbeef;
  }

  int nupid = pin->pid();
  int ccpid = nupid > 0 ? (nupid - 1) : (nupid + 1);

  auto pout = ps::sel::OutPartFirstAny(ev, {ccpid, nupid});

  return parts::q0(pin, pout);
}

// event::q3(event) -> real
double q3(HepMC3::GenEvent const &ev) {

  auto pin = ps::sel::BeamAny(ev, pdg::groups::kNeutralLeptons);

  if (!pin) {
    return 0xdeadbeef;
  }

  int nupid = pin->pid();
  int ccpid = nupid > 0 ? (nupid - 1) : (nupid + 1);

  auto pout = ps::sel::OutPartFirstAny(ev, {ccpid, nupid});

  return parts::q3(pin, pout);
}

// event::Q2Lep(event) -> real
double Q2Lep(HepMC3::GenEvent const &ev) {

  auto pin = ps::sel::BeamAny(ev, pdg::groups::kNeutralLeptons);

  if (!pin) {
    return 0xdeadbeef;
  }

  int nupid = pin->pid();
  int ccpid = nupid > 0 ? (nupid - 1) : (nupid + 1);

  auto pout = ps::sel::OutPartFirstAny(ev, {ccpid, nupid});

  return parts::Q2(pin, pout);
}

// event::CosThetaLep(event) -> real
double CosThetaLep(HepMC3::GenEvent const &ev) {

  auto pin = ps::sel::BeamAny(ev, pdg::groups::kNeutralLeptons);

  if (!pin) {
    return 0xdeadbeef;
  }

  int nupid = pin->pid();
  int ccpid = nupid > 0 ? (nupid - 1) : (nupid + 1);

  auto pout = ps::sel::OutPartFirstAny(ev, {ccpid, nupid});

  return parts::CosTheta(pin, pout);
}

// event::ThetaLep(event) -> real
double ThetaLep(HepMC3::GenEvent const &ev) {

  auto pin = ps::sel::BeamAny(ev, pdg::groups::kNeutralLeptons);

  if (!pin) {
    return 0xdeadbeef;
  }

  int nupid = pin->pid();
  int ccpid = nupid > 0 ? (nupid - 1) : (nupid + 1);

  auto pout = ps::sel::OutPartFirstAny(ev, {ccpid, nupid});

  return parts::Theta(pin, pout);
}

} // namespace event
} // namespace proj
} // namespace ps