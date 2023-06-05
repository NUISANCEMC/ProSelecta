#pragma once

#include <vector>

namespace ps {

namespace pdg {
static int const kNuE = 12;
static int const kANuE = -12;

static int const kNuMu = 14;
static int const kANuMu = -14;

static int const kElectron = 11;
static int const kAElectron = -11;

static int const kMuon = 13;
static int const kAMuon = -13;

static int const kPiPlus = 211;
static int const kPiZero = 111;
static int const kPiMinus = -211;

static int const kProton = 2212;
static int const kAProton = -2212;

static int const kNeutron = 2112;
static int const kANeutron = -2112;

namespace groups {

static std::vector<int> kChargedLeptons = {kElectron, kAElectron, kMuon,
                                           kAMuon};
static std::vector<int> kChargedLeptons_matter = {kElectron, kMuon};
static std::vector<int> kChargedLeptons_amatter = {kAElectron, kAMuon};

static std::vector<int> kNeutralLeptons = {kNuE, kANuE, kNuMu, kANuMu};
static std::vector<int> kNeutralLeptons_matter = {kNuE, kNuMu};
static std::vector<int> kNeutralLeptons_amatter = {kANuE, kANuMu};

static std::vector<int> kLeptons = {kElectron, kAElectron, kMuon, kAMuon,
                                    kNuE,      kANuE,      kNuMu, kANuMu};

} // namespace groups

} // namespace pdg

} // namespace ps