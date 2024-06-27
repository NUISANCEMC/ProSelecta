#pragma once

#include <array>
#include <vector>

namespace ps {

template <typename T, std::size_t N> using pids = std::array<T, N>;

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

constexpr static pids kChargedLeptons = {kElectron, kAElectron, kMuon, kAMuon};
constexpr static pids kChargedLeptons_matter = {kElectron, kMuon};
constexpr static pids kChargedLeptons_amatter = {kAElectron, kAMuon};

constexpr static pids kNeutralLeptons = {kNuE, kANuE, kNuMu, kANuMu};
constexpr static pids kNeutralLeptons_matter = {kNuE, kNuMu};
constexpr static pids kNeutralLeptons_amatter = {kANuE, kANuMu};

constexpr static pids kLeptons = {kElectron, kAElectron, kMuon, kAMuon,
                                  kNuE,      kANuE,      kNuMu, kANuMu};

} // namespace pdg
} // namespace ps