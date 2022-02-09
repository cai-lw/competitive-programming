#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <iostream>
#include "cplib/port/bit.hpp"

namespace cplib {

// Op must be associative, commutative and idempotent
template<typename T, typename Op>
struct SparseTable {
    std::vector<std::vector<T>> table;
    Op op;

    using size_type = std::size_t;

    SparseTable() {}

    SparseTable(std::vector<T> &&data) : op() {
        table.emplace_back(data);
        _build();
    }

    template<typename InputIt>
    SparseTable(InputIt first, InputIt last) : op() {
        table.emplace_back(first, last);
        _build();
    }

    size_type size() {
        return table[0].size();
    }

    T range(size_type left, size_type right) {
        int level = port::bit_width(right - left) - 1;
        size_type left2 = right - (size_type(1) << level);
        if (left == left2)
            return table[level][left];
        else
            return op(table[level][left], table[level][left2]);
    }

    T range_inclusive(size_type left, size_type right) {
        return range(left, right + 1);
    }

private:
    void _build() {
        size_type input_size = table.back().size();
        while (input_size > (size_type(1) << table.size()) - 1) {
            int level = table.size();
            table.emplace_back();
            size_type level_size = input_size + 1 - (size_type(1) << level);
            table.back().reserve(level_size);
            for (size_type i = 0; i < level_size; i++) {
                table.back().push_back(
                    op(table[level - 1][i], table[level - 1][i + (size_type(1) << (level - 1))]));
            }
        }
    }
};

}  // namespace cplib