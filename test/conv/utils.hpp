#include <vector>

template <typename ModInt>
inline std::vector<ModInt> from_int_vec(const std::vector<int> &a) {
  std::vector<ModInt> res;
  for (int x : a) {
    res.push_back(ModInt(x));
  }
  return res;
}

template <typename ModInt>
inline std::vector<int> to_int_vec(const std::vector<ModInt> &a) {
  std::vector<int> res;
  for (const ModInt &x : a) {
    res.push_back(x.val());
  }
  return res;
}