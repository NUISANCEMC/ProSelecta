#pragma once

#include <array>
#include <vector>

namespace ps {

template <typename... Args> constexpr auto pids(Args... args) {
  return std::array{args...};
}

namespace pdg {

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic push

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

constexpr static std::array kChargedLeptons = {kElectron, kAElectron, kMuon,
                                               kAMuon};
constexpr static std::array kChargedLeptons_matter = {kElectron, kMuon};
constexpr static std::array kChargedLeptons_amatter = {kAElectron, kAMuon};

constexpr static std::array kNeutralLeptons = {kNuE, kANuE, kNuMu, kANuMu};
constexpr static std::array kNeutralLeptons_matter = {kNuE, kNuMu};
constexpr static std::array kNeutralLeptons_amatter = {kANuE, kANuMu};

constexpr static std::array kLeptons = {kElectron, kAElectron, kMuon, kAMuon,
                                        kNuE,      kANuE,      kNuMu, kANuMu};

constexpr static std::array kPions = {kPiPlus, kPiZero, kPiMinus};

// ±10LZZZAAAI
constexpr static int Carbon12 = 1000060120;
constexpr static int Oxygen16 = 1000080160;
constexpr static int Argon40 = 1000180400;
constexpr static int Iron56 = 1000260560;

#pragma GCC diagnostic pop

} // namespace pdg
} // namespace ps