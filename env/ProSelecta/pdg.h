#pragma once

#include <vector>

namespace ps {
namespace pdg {
constexpr static int kNuE = 12;
constexpr static int kANuE = -12;

constexpr static int kNuMu = 14;
constexpr static int kANuMu = -14;

constexpr static int kElectron = 11;
constexpr static int kAElectron = -11;

constexpr static int kMuon = 13;
constexpr static int kAMuon = -13;

constexpr static int kPiPlus = 211;
constexpr static int kPiZero = 111;
constexpr static int kPiMinus = -211;

constexpr static int kProton = 2212;
constexpr static int kAProton = -2212;

constexpr static int kNeutron = 2112;
constexpr static int kANeutron = -2112;

constexpr static int kGamma = 22;

constexpr static int kNuclearPDGBoundary = 1000000000;

static std::vector<int> kChargedLeptons = {kElectron, kAElectron, kMuon,
                                           kAMuon};
static std::vector<int> kChargedLeptons_matter = {kElectron, kMuon};
static std::vector<int> kChargedLeptons_amatter = {kAElectron, kAMuon};

static std::vector<int> kNeutralLeptons = {kNuE, kANuE, kNuMu, kANuMu};
static std::vector<int> kNeutralLeptons_matter = {kNuE, kNuMu};
static std::vector<int> kNeutralLeptons_amatter = {kANuE, kANuMu};

static std::vector<int> kLeptons = {kElectron, kAElectron, kMuon, kAMuon,
                                    kNuE,      kANuE,      kNuMu, kANuMu};

} // namespace pdg
} // namespace ps