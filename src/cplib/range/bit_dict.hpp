#pragma once

#include <climits>
#include <cstdint>
#include <memory>
#include <vector>
#include "cplib/port/bit.hpp"
#include "cplib/utils/bit.hpp"

namespace cplib {

namespace impl {

class alignas(64) BitDictBlock {
public:
    using size_type = std::size_t;
    using word_type = std::uint64_t;
    static constexpr size_type words_per_block = 6;
    static constexpr size_type word_size = std::numeric_limits<word_type>::digits;
    static constexpr size_type block_size = word_size * words_per_block;

    BitDictBlock() : data_({0}) {}

    size_type build(size_type block_rank) {
        block_rank_ = block_rank;
        wrap_word_ = words_per_block;
        size_type acc_rank = 0;
        for (int i = 0; i < words_per_block; i++) {
            if (acc_rank >= 256 && wrap_word_ == words_per_block) {
                wrap_word_ = i;
            }
            word_rank_[i] = acc_rank;
            acc_rank += port::popcount(data_[i]);
        }
        return block_rank_ + acc_rank;
    }

    bool get(size_type idx) const {
        return (data_[idx / word_size] >> (idx % word_size)) & 1;
    }

    void set(size_type idx) {
        data_[idx / word_size] |= word_type(1) << (idx % word_size);
    }

    size_type rank(size_type idx) const {
        size_type word_idx = idx / word_size;
        return block_rank_ + get_word_rank_(word_idx) + impl::popcount_low(data_[word_idx], idx % word_size);
    }

private:
    size_type get_word_rank_(size_type idx) const {
        return word_rank_[idx] + (idx >= wrap_word_) * 256;
    }

    std::array<word_type, words_per_block> data_;
    size_type block_rank_;
    std::array<std::uint8_t, words_per_block> word_rank_;
    std::uint8_t wrap_word_;
};

static_assert(sizeof(BitDictBlock) == 64);

}  // namespace impl

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
 * This implementation uses cache line aligned blocks to ensure that each rank query reads exactly one cache line.
 */
class BitDict {
public:
    using size_type = impl::BitDictBlock::size_type;

    /** \brief Creates an empty BitDict */
    BitDict() : size_(0), zeros_(0) {}

    /**
     * \brief Creates a BitDict with `num_bits` bits, all initialized to 0.
     * 
     * Use set() or fill_with_bit_generator() to set its content.
     */
    BitDict(size_type num_bits) : size_(num_bits), blocks_(num_bits / block_size + 1) {}

    /** \brief Returns the number of bits. */
    size_type size() const { return size_; }

    /** \brief Returns the number of zero bits. */
    size_type zeros() const { return zeros_; }

    /** \brief Returns the number of one bits. */
    size_type ones() const { return size_ - zeros_; }

    /** \brief Read-only access to a bit. Requires `0 <= idx < size()` */
    bool get(size_type idx) const {
        return blocks_[idx / block_size].get(idx % block_size);
    }

    /**
     * \brief Set a bit to 1. Requires `0 <= idx < size()`.
     * 
     * Can only be called before build().
     */
    void set(size_type idx) {
        blocks_[idx / block_size].set(idx % block_size);
    }

    /**
     * \brief Overwrite all bits with a bit generator.
     * 
     * Can only be called before build(). `gen` will be called exactly `size()` times.
     */
    template<typename BitGenerator>
    void fill_with_bit_generator(BitGenerator& gen) {
        for (size_type i = 0; i < size_; i++) {
            if (gen()) {
                set(i);
            }
        }
    }

    /** \brief Set up auxiliary data to prepare for rank queries. */
    void build() {
        size_type acc_rank = 0;
        for (auto& block : blocks_) {
            acc_rank = block.build(acc_rank);
        }
        zeros_ = size_ - acc_rank;
    }

    /**
     * \brief Returns the number of 1 bits in the range `[0, idx)`.
     * 
     * Can only be called after build(). Requires `0 <= idx <= size()`. Runs in \f$O(1)\f$ time.
     */
    size_type rank1(size_type idx) const {
        return blocks_[idx / block_size].rank(idx % block_size);
    }

    /** \brief Returns the number of 0 bits in the range `[0, idx)`.
     * \copydetails rank1 */
    size_type rank0(size_type idx) const {
        return idx - rank1(idx);
    }

    /**
     * \brief Move one level downwards in a wavelet tree.
     * 
     * \copydetails rank1
     * 
     * This method answers the following question: "If this BitDict represents a level in a wavelet tree, for a boundary
     * located right before `idx`, where is the corresponding boundary at the 0 or 1 child in the next level?".
     * It turns out that the answer does not depend on the structure of the wavelet tree. As a corollary, an interval
     * `[left, right)` becomes `[rank_to_child(left, bit), rank_to_child(right, bit))` in the next level, where `bit`
     * means taking the 0-branch or the 1-branch.
     */
    size_type rank_to_child(size_type idx, bool bit) const {
        return bit ? zeros() + rank1(idx) : rank0(idx);
    }

private:
    static constexpr size_type block_size = impl::BitDictBlock::block_size;
    std::vector<impl::BitDictBlock> blocks_;
    size_type size_, zeros_;
};

}  // namespace cplib