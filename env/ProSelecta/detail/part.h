#pragma once

#include <vector>

namespace ps::detail {
template <typename PartCollectionCollection>
inline auto cat(PartCollectionCollection const &part_groups) {

  std::vector<typename PartCollectionCollection::value_type::value_type> outs;
  for (auto const &parts : part_groups) {
    for (auto const &p : parts) {
      outs.push_back(p);
    }
  }
  return outs;
}
} // namespace ps::detail