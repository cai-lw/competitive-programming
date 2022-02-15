#pragma once

#include <limits>
#include <type_traits>
#include "cplib/pragmas.h"

// Backport some functions in <bit> in C++20.

namespace cplib::port {

namespace impl {

template<typename T>
constexpr bool is_unsigned_integer_v = std::is_unsigned_v<T> && !std::is_same_v<T, bool>;

template<typename T>
constexpr bool long_long_or_smaller = std::numeric_limits<T>::digits <= std::numeric_limits<unsigned long long>::digits;

template<typename T>
constexpr bool int_or_smaller = std::numeric_limits<T>::digits <= std::numeric_limits<unsigned int>::digits;

}  // namespace impl

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr int countl_zero(T x) noexcept {
    static_assert(impl::long_long_or_smaller<T>);
    if (x == 0) {
        return std::numeric_limits<T>::digits;
    } else if (impl::int_or_smaller<T>) {
        return __builtin_clz(x) - std::numeric_limits<unsigned int>::digits + std::numeric_limits<T>::digits;
    } else {
        return __builtin_clzll(x) - std::numeric_limits<unsigned long long>::digits + std::numeric_limits<T>::digits;
    }
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr int countr_zero(T x) noexcept {
    static_assert(impl::long_long_or_smaller<T>);
    if (x == 0) {
        return std::numeric_limits<T>::digits;
    } else if (impl::int_or_smaller<T>) {
        return __builtin_ctz(x);
    } else {
        return __builtin_ctzll(x);
    }
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr T bit_floor(T x) noexcept {
    return x == 0 ? 0 : T(1) << (bit_width(x) - 1);
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr T bit_ceil(T x) noexcept {
    return x <= T(1) ? T(1) : T(1) << bit_width(x - 1);
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr bool has_single_bit(T x) noexcept {
    return x != 0 && (x & (x - 1)) == 0;
};

template<typename T, std::enable_if_t<impl::is_unsigned_integer_v<T>>* = nullptr>
constexpr int popcount(T x) noexcept {
    static_assert(impl::long_long_or_smaller<T>);
    if (impl::int_or_smaller<T>) {
        return __builtin_popcount(x);
    } else {
        return __builtin_popcountll(x);
    }
};

}  // namespace cplib::port