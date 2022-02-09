#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <limits>
#include "cplib/port/bit.hpp"
#include "cplib/range/sparse_table.hpp"

namespace cplib {

struct RangeMinBlock {
    using bitmap_t = std::uint_fast32_t;
    static constexpr int bitmap_size = std::numeric_limits<bitmap_t>::digits;

    RangeMinBlock() {}

    template<typename InputIt, typename Comp>
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

    int min_idx_inclusive(int left, int right) {
        bitmap_t loc = min_loc[right] & ~((bitmap_t(1) << left) - 1);
        return loc == 0 ? right : port::countr_zero(loc);
    }
private:
    // min_loc[j][i]=1 if and only if A[i] is the smallest among A[i..j] (inclusive)
    std::array<bitmap_t, bitmap_size> min_loc;
};

template<typename T, typename Comp = std::less<T>>
struct RangeMinQuery {
    struct MinOp {
        Comp comp;
        const T& operator()(const T& a, const T& b) {
            return comp(a, b) ? a : b;
        }
    };

    static constexpr int block_size = RangeMinBlock::bitmap_size;
    using size_type = std::size_t;

    SparseTable<T, MinOp> block_table;
    std::vector<RangeMinBlock> blocks;
    std::vector<T> data;
    MinOp min_op;

    RangeMinQuery(const std::vector<T> &arr) : data(arr) { _build(); }

    RangeMinQuery(std::vector<T> &&arr) : data(arr) { _build(); }

    template<typename InputIt>
    RangeMinQuery(InputIt first, InputIt last) : data(first, last) { _build(); }

    size_type size() { return data.size(); }

    T range_min(size_type left, size_type right) {
        return range_min_inclusive(left, right - 1);
    }

    T range_min_inclusive(size_type left, size_type right) {
        size_type left_block = left / block_size, right_block = right / block_size;
        if (left_block == right_block) {
            int block_idx = blocks[left_block].min_idx_inclusive(left % block_size, right % block_size);
            return data[left_block * block_size + block_idx];
        }
        int left_block_idx = blocks[left_block].min_idx_inclusive(left % block_size, block_size - 1);
        int right_block_idx = blocks[right_block].min_idx_inclusive(0, right % block_size);
        const T &left_block_min = data[left_block * block_size + left_block_idx];
        const T &right_block_min = data[right_block * block_size + right_block_idx];
        const T &lr_block_min = min_op(left_block_min, right_block_min);
        if (left_block + 1 == right_block) {
            return lr_block_min;
        } else {
            return min_op(lr_block_min, block_table.range_inclusive(left_block + 1, right_block - 1));
        }
    }

private:
    void _build() {
        size_type num_blocks = (data.size() + block_size - 1) / block_size;
        Comp comp;
        blocks.reserve(num_blocks);
        for (size_type i = 0; i < num_blocks; i++) {
            auto first = data.begin() + i * block_size;
            auto last = (i + 1) * block_size >= data.size() ? data.end() : first + block_size;
            blocks.emplace_back(first, last, comp);
        }
        std::vector<T> block_min;
        block_min.reserve(num_blocks);
        for (size_type i = 0; i < num_blocks; i++)
            block_min.push_back(data[i * block_size + blocks[i].min_idx_inclusive(0, block_size - 1)]);
        block_table = SparseTable<T, MinOp>(move(block_min));
    }
};

}  // namespace cplib