#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

namespace cplib {

namespace impl {

template <typename Range, typename Element>
inline constexpr bool is_static_query_range_v = std::is_invocable_v<decltype(&Range::push_front), Range&, Element> &&
                                                std::is_invocable_v<decltype(&Range::pop_front), Range&, Element> &&
                                                std::is_invocable_v<decltype(&Range::push_back), Range&, Element> &&
                                                std::is_invocable_v<decltype(&Range::pop_back), Range&, Element> &&
                                                std::is_invocable_v<decltype(&Range::get), Range&>;

}

/**
 * \brief Batch range queries using Mo's algorithm
 * \ingroup range
 *
 * Given a `Range` object that supports pushing and popping elements on both ends and querying some property about the
 * range it currently holds, this functions answers \f$Q\f$ range queries for that property given as subranges of a
 * range of length \f$N\f$, calling `Range`'s push/pop methods \f$O(Q\sqrt{N})\f$ times.
 *
 * \tparam Range must have the following methods, where `T` is the value type of the queries' iterator type
 * * `void Range::push_front(const T&)`
 * * `void Range::push_back(const T&)`
 * * `void Range::pop_front(const T&)`
 * * `void Range::pop_back(const T&)`
 * * `R Range::get()`
 */
template <typename RandomIt, typename Range,
          std::enable_if_t<impl::is_static_query_range_v<Range, typename std::iterator_traits<RandomIt>::value_type>>* =
              nullptr>
std::vector<decltype(std::declval<Range>().get())> batch_range_queries(
    Range& range, const std::vector<std::pair<RandomIt, RandomIt>>& queries) {
  if (queries.empty()) {
    return {};
  }
  std::vector<size_t> order(queries.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](size_t i, size_t j) { return queries[i].first < queries[j].first; });
  auto leftmost = std::min_element(queries.begin(), queries.end(), [](const auto& a, const auto& b) {
                    return a.first < b.first;
                  })->first;
  auto rightmost = std::max_element(queries.begin(), queries.end(), [](const auto& a, const auto& b) {
                     return a.second < b.second;
                   })->second;
  size_t block_size = std::max(size_t(1), size_t((rightmost - leftmost) / std::sqrt(queries.size())));
  bool block_reverse = false;
  auto block_sort_cmp = [&](size_t i, size_t j) {
    return block_reverse ? queries[i].second > queries[j].second : queries[i].second < queries[j].second;
  };
  auto block_begin = order.begin();
  for (auto block_end = block_begin; block_end < order.end(); block_end++) {
    if (queries[*block_end].first - queries[*block_begin].first >= block_size) {
      std::sort(block_begin, block_end, block_sort_cmp);
      block_reverse ^= 1;
      block_begin = block_end;
    }
  }
  std::sort(block_begin, order.end(), block_sort_cmp);
  RandomIt l = queries[order[0]].first, r = l;
  std::vector<decltype(std::declval<Range>().get())> ans(queries.size());
  for (auto i : order) {
    auto [l_target, r_target] = queries[i];
    while (l > l_target) {
      range.push_front(*(--l));
    }
    while (r < r_target) {
      range.push_back(*(r++));
    }
    while (l < l_target) {
      range.pop_front(*(l++));
    }
    while (r > r_target) {
      range.pop_back(*(--r));
    }
    ans[i] = range.get();
  }
  return ans;
}

}  // namespace cplib