#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "cplib/port/bit.hpp"

namespace cplib {

/**
 * \brief \f$\langle O(N\log N), O(1) \rangle\f$ sparse table for certain binary operations like `min` and `gcd`.
 * \ingroup range
 *
 * \tparam T T Type of elements.
 * \tparam Op A function object that takes two `T`'s and returns a `T`. Must be associative, commutative and idempotent.
 */
template <typename T, typename Op>
class SparseTable {
 public:
  using size_type = std::size_t;

  /** \brief Creates an empty sparse table. */
  SparseTable() = default;

  /** \copydoc SparseTable(InputIt, InputIt) */
  SparseTable(const std::vector<T> &data) : op() {
    table.emplace_back(data);
    _build();
  }

  /** \copydoc SparseTable(InputIt, InputIt) */
  SparseTable(std::vector<T> &&data) : op() {
    table.emplace_back(std::move(data));
    _build();
  }

  /**
   * \brief Construct the sparse table from the given sequence.
   *
   * Makes \f$O(N\log N)\f$ calls to `Op` and creates \f$O(N\log N)\f$ copies of sequence elements.
   */
  template <typename InputIt>
  SparseTable(InputIt first, InputIt last) : op() {
    table.emplace_back(first, last);
    _build();
  }

  /** \brief Returns number of elements in the sequence */
  size_type size() const { return table[0].size(); }

  /**
   * \brief Iterated application of operator `Op` on elements in the 0-based half-open range `[left, right)`.
   *
   * If \f$\circ\f$ denotes `Op`, this returns \f$a_{left} \cdots a_{left+1} \circ \dots \circ a_{right-1}\f$.
   * Time complexity is \f$O(1)\f$ and specfically `Op` is called at most once.
   *
   * Requires `0 <= left < right <= size()`. Note that empty range is not allowed.
   */
  T range(size_type left, size_type right) const {
    int level = port::bit_width(right - left) - 1;
    size_type left2 = right - (size_type(1) << level);
    if (left == left2)
      return table[level][left];
    else
      return op(table[level][left], table[level][left2]);
  }

 private:
  std::vector<std::vector<T>> table;
  Op op;

  void _build() {
    size_type input_size = table.back().size();
    while (input_size > (size_type(1) << table.size()) - 1) {
      int level = table.size();
      table.emplace_back();
      size_type level_size = input_size + 1 - (size_type(1) << level);
      table.back().reserve(level_size);
      for (size_type i = 0; i < level_size; i++) {
        table.back().push_back(op(table[level - 1][i], table[level - 1][i + (size_type(1) << (level - 1))]));
      }
    }
  }
};

}  // namespace cplib