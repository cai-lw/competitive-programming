#pragma once

#include <climits>
#include <cstdint>
#include <memory>
#include <vector>
#include <x86intrin.h>
#include "cplib/port/bit.hpp"
#include "cplib/utils/bit.hpp"

namespace cplib {

/**
 * \brief Static bit sequence with rank query in \f$O(1)\f$.
 * \ingroup range
 * 
 * Together with *select* query, this data structure is known as Fully Indexable Dictionary. Theoretical results
 * using \f$o(N)\f$ space and \f$O(1)\f$ time for both queries are known, but are of little practical interest.
 * Practical implementations usually target large datasets on the order of \f$10^8\f$ or more bits, where memory access
 * is the bottleneck. In competitive programming, however, the problem size is usually no more than \f$10^6\f$,
 * and memory is much cheaper.
 * 
 * Based on benchmakrk results, the fastest implementation for competitive programming is simply storing the rank for
 * each 64-bit word. That is, the block size is one word, and there is only one level of index.
 */
class BitDict {
public:
    using size_type = uint32_t;
    using word_t = uint64_t;
    static constexpr int word_size = std::numeric_limits<word_t>::digits;

    /** \brief Creates an empty BitDict */
    BitDict() : size_(0), zeros_(0) {}

    /**
     * \brief Creates a BitDict with the given contiguously stored bit sequence.
     * 
     * BitDictBuilder is usually prefered over using this constructor. Runs in \f$O(N)\f$ time.
     */
    BitDict(std::vector<word_t> &&data, size_type num_bits)
        : data(std::move(data)), size_(num_bits) { _build(); }

    /** \brief Returns the number of bits. */
    size_type size() const { return size_; }

    /** \brief Returns the number of zero bits. */
    size_type zeros() const { return zeros_; }

    /** \brief Returns the number of one bits. */
    size_type ones() const { return size_ - zeros_; }

    /** \brief Read-only access to a bit. */
    bool get(size_type idx) const {
        std::size_t word_idx = idx / word_size;
        int bits_in_word = idx % word_size;
        return (data[word_idx] >> bits_in_word) & 1;
    }

    /**
     * \brief Returns the number of 1 bits in the range `[0, idx)`.
     * 
     * Requires `0 <= idx <= size()`. Runs in \f$O(1)\f$ time.
     */
    size_type rank1(size_type idx) const {
        std::size_t word_idx = idx / word_size;
        int bit_idx = idx % word_size;
        // Check is necessary because data[word_idx] may be out of bounds if idx == size()
        if (bit_idx != 0) {
            return word_rank[word_idx] + impl::popcount_low(data[word_idx], bit_idx);
        } else {
            return word_rank[word_idx];
        }
    }

    /** \brief Returns the number of 0 bits in the range `[0, idx)`.
     * \copydetails rank1 */
    size_type rank0(size_type idx) const {
        return idx - rank1(idx);
    }

    /**
     * \brief Move one level downwards in a wavelet tree.
     * 
     * This method answers the following question: "If this BitDict represents a level in a wavelet tree, for a boundary
     * located right before `idx`, where is the corresponding boundary at the 0 or 1 child in the next level?".
     * It turns out that the answer does not depend on the structure of the wavelet tree. As a corollary, an interval
     * `[left, right)` becomes `[rank_to_child(left, bit), rank_to_child(right, bit))` in the next level, where `bit`
     * means taking the 0-branch or the 1-branch.
     * 
     * \copydetails rank1
     */
    size_type rank_to_child(size_type idx, bool bit) const {
        return bit ? zeros() + rank1(idx) : rank0(idx);
    }

private:
    std::vector<word_t> data;
    std::vector<size_type> word_rank;
    size_type size_, zeros_;

    void _build() {
        std::size_t last_word = size_ / word_size;
        word_rank.resize(last_word + 1);
        for (std::size_t i = 0; i < last_word; i++) {
            word_rank[i + 1] = word_rank[i] + port::popcount(data[i]);
        }
        zeros_ = size_ - rank1(size_);
    }
};

/**
 * \brief Builder class for BitDict.
 * \ingroup range
 * 
 * Unless you already have the bits stored contiguously, this should be the preferred way to build a BitDict.
 */
class BitDictBuilder {
public:
    using size_type = BitDict::size_type;

    BitDictBuilder() : size(0) {}

    /**
     * \brief Construct a BitDictBuilder with the given number of bits.
     * 
     * Note that the size cannot be changed after construction. The BitDict created by build() will have the same
     * number of bits. All bits are initialized to 0.
     */
    BitDictBuilder(size_type num_bits) :
        size(num_bits), data((num_bits + BitDict::word_size - 1) / BitDict::word_size) {}

    /**
     * \brief Set the given bit to 1 (default) or zero
     * 
     * Requires `0 <= idx < size()`.
     */
    void set(size_type idx, bool bit = true) {
        std::size_t word_idx = idx / BitDict::word_size;
        int bit_in_word = idx % BitDict::word_size;
        data[word_idx] |= BitDict::word_t(bit) << bit_in_word;
    }

    /** \brief Construct a BitDict, moving the ownership of the underlying data to it. */
    BitDict build() { return BitDict(std::move(data), size); }

private:
    std::vector<BitDict::word_t> data;
    size_type size;
};

}  // namespace cplib