#pragma once

#include <functional>
#include <string>
#include <type_traits>

namespace HepMC3 {
class GenEvent;
}

namespace ps {
using SelectFunc = std::function<int(HepMC3::GenEvent const &)>;
using SelectsFunc = std::function<std::vector<int>(HepMC3::GenEvent const &)>;
using ProjectionFunc = std::function<double(HepMC3::GenEvent const &)>;
using ProjectionsFunc =
    std::function<std::vector<double>(HepMC3::GenEvent const &)>;
using WeightFunc = ProjectionFunc;
} // namespace ps