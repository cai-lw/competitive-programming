#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <limits>
#include <vector>

#include "cplib/port/bit.hpp"
#include "cplib/range/sparse_table.hpp"

namespace cplib {

namespace impl {

class RangeMinBlock {
 public:
  using bitmap_t = uint64_t;
  static constexpr int bitmap_size = std::numeric_limits<bitmap_t>::digits;

  template <typename InputIt, typename Comp>
  RangeMinBlock(InputIt first, InputIt last, Comp comp) {
    using T = typename std::iterator_traits<InputIt>::value_type;
    // Keep a static monotonic stack to avoid reallocation
    static std::vector<std::pair<T, int>> monotonic_stack;
    monotonic_stack.clear();
    int idx = 0;
    for (auto it = first; it != last; it++) {
      while (!monotonic_stack.empty() && comp(*it, monotonic_stack.back().first)) {
        monotonic_stack.pop_back();
      }
      if (monotonic_stack.empty()) {
        min_loc[idx] = 0;
      } else {
        int prev_idx = monotonic_stack.back().second;
        min_loc[idx] = min_loc[prev_idx] | (bitmap_t(1) << prev_idx);
      }
      monotonic_stack.emplace_back(*it, idx);
      idx++;
    }
  }

  int min_idx_inclusive(int left, int right) const {
    bitmap_t loc = min_loc[right] & ~((bitmap_t(1) << left) - 1);
    return loc == 0 ? right : port::countr_zero(loc);
  }

 private:
  // min_loc[j][i]=1 if and only if A[i] is the smallest among A[i..j] (inclusive)
  std::array<bitmap_t, bitmap_size> min_loc;
};

template <typename T, typename Comp>
struct MinOp {
  Comp comp;
  const T& operator()(const T& a, const T& b) const { return std::min(a, b, comp); }
};

}  // namespace impl

/**
 * \brief Efficient \f$\langle O(N), O(1) \rangle\f$ static range minimum query.
 * \ingroup range
 *
 * Divides input sequence into machine word sized blocks. Uses bit operations for in-block queries, and SparseTable for
 * inter-block queries. Under the transdichotomous model (\f$w\geq\log_2 N\f$ where \f$w=64\f$ is the word size),
 * initialization takes \f$O(N)\f$ time.
 *
 * \tparam T Type of elements.
 * \tparam Comp Comparison function type that is passed to `std::min`. Must satisfy C++ named requirement
 * [*Compare*](https://en.cppreference.com/w/cpp/named_req/Compare).
 */
template <typename T, typename Comp = std::less<T>>
class RangeMinQuery {
 public:
  static constexpr int block_size = impl::RangeMinBlock::bitmap_size;
  using size_type = std::size_t;

  /** \brief Creates an empty RangeMinQuery. */
  RangeMinQuery() = default;

  /** \copydoc RangeMinQuery::RangeMinQuery(InputIt, InputIt) */
  RangeMinQuery(const std::vector<T>& arr) : data(arr) { _build(); }

  /** \copydoc RangeMinQuery::RangeMinQuery(InputIt, InputIt) */
  RangeMinQuery(std::vector<T>&& arr) : data(std::move(arr)) { _build(); }

  /**
   * \brief Construct the range minumum query object from the given sequence.
   *
   * Makes \f$O(N)\f$ calls to `Comp` and creates \f$O(N)\f$ copies of sequence elements along with
   * \f$O(N)\f$ integers as bit masks.
   */
  template <typename InputIt>
  RangeMinQuery(InputIt first, InputIt last) : data(first, last) {
    _build();
  }

  /** \brief Returns number of elements in the sequence */
  size_type size() const { return data.size(); }

  /**
   * \brief Returns the minimum element in the 0-based half-open range `[left, right)`.
   *
   * Returns \f$\min\{a_{left},a_{left+1},\dots,a_{right-1}\}\f$.
   * Time complexity is \f$O(1)\f$ and specifically `Comp` is called at most 3 times.
   *
   * Requires `0 <= left < right <= size()`. Note that empty range is not allowed.
   */
  T range_min(size_type left, size_type right) const { return _range_min_inclusive(left, right - 1); }

 private:
  SparseTable<T, impl::MinOp<T, Comp>> block_table;
  std::vector<impl::RangeMinBlock> blocks;
  std::vector<T> data;
  Comp comp;

  void _build() {
    size_type num_blocks = (data.size() + block_size - 1) / block_size;
    Comp comp;
    blocks.reserve(num_blocks);
    for (size_type i = 0; i + 1 < num_blocks; i++) {
      auto begin = data.begin() + i * block_size;
      blocks.emplace_back(begin, begin + block_size, comp);
    }
    blocks.emplace_back(data.begin() + (num_blocks - 1) * block_size, data.end(), comp);
    std::vector<T> block_min;
    block_min.reserve(num_blocks);
    for (size_type i = 0; i + 1 < num_blocks; i++) {
      block_min.push_back(data[i * block_size + blocks[i].min_idx_inclusive(0, block_size - 1)]);
    }
    block_min.push_back(data[(num_blocks - 1) * block_size +
                             blocks.back().min_idx_inclusive(0, (data.size() + block_size - 1) % block_size)]);
    block_table = SparseTable<T, impl::MinOp<T, Comp>>(std::move(block_min));
  }

  T _range_min_inclusive(size_type left, size_type right) const {
    size_type left_block = left / block_size, right_block = right / block_size;
    if (left_block == right_block) {
      int block_idx = blocks[left_block].min_idx_inclusive(left % block_size, right % block_size);
      return data[left_block * block_size + block_idx];
    }
    int left_block_idx = blocks[left_block].min_idx_inclusive(left % block_size, block_size - 1);
    int right_block_idx = blocks[right_block].min_idx_inclusive(0, right % block_size);
    const T& left_block_min = data[left_block * block_size + left_block_idx];
    const T& right_block_min = data[right_block * block_size + right_block_idx];
    const T& lr_block_min = std::min(left_block_min, right_block_min, comp);
    if (left_block + 1 == right_block) {
      return lr_block_min;
    } else {
      return std::min(lr_block_min, block_table.range(left_block + 1, right_block), comp);
    }
  }
};

}  // namespace cplib