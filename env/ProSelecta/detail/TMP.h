#pragma once

#include "HepMC3/GenParticle.h"

#include <array>
#include <type_traits>
#include <vector>

namespace ps::detail {

template <typename Collection> struct is_std_array : std::false_type {};
template <typename T, size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename Collection> struct is_std_vector : std::false_type {};
template <typename T> struct is_std_vector<std::vector<T>> : std::true_type {};

template <typename Collection> struct is_zero_std_array : std::false_type {};

template <size_t N> struct is_zero_std_array<std::array<int, N>> {
  constexpr static bool value = (N == 0);
};

template <typename Collection> struct is_std_array_int : std::false_type {};
template <size_t N>
struct is_std_array_int<std::array<int, N>> : std::true_type {};

template <typename Collection> struct is_std_vector_int : std::false_type {};
template <> struct is_std_vector_int<std::vector<int>> : std::true_type {};

template <typename Collection> struct is_std_vector_or_array_int {
  constexpr static bool value = is_std_array_int<Collection>::value ||
                                is_std_vector_int<Collection>::value;
};

template <typename Collection> struct is_std_array_part : std::false_type {};
template <size_t N>
struct is_std_array_part<std::array<HepMC3::ConstGenParticlePtr, N>>
    : std::true_type {};

template <typename Collection> struct is_std_vector_part : std::false_type {};
template <>
struct is_std_vector_part<std::vector<HepMC3::ConstGenParticlePtr>>
    : std::true_type {};

template <typename Collection> struct is_std_vector_or_array_part {
  constexpr static bool value = is_std_array_part<Collection>::value ||
                                is_std_vector_part<Collection>::value;
};

template <typename Collection, typename ValueType> struct broadcast_return {};

template <size_t N, typename ValueType>
struct broadcast_return<std::array<int, N>, ValueType> {
  using type = std::array<ValueType, N>;
};

template <typename ValueType>
struct broadcast_return<std::vector<int>, ValueType> {
  using type = std::vector<ValueType>;
};

struct squeeze {};

} // namespace ps::detail