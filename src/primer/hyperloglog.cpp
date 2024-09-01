#include "primer/hyperloglog.h"
#include <algorithm>
#include <bitset>
#include <cmath>
#include <mutex>
#include <string>
#include <type_traits>

namespace bustub {

template <typename T>
auto inline HyperLogLog<T>::CalculateHash(T val) -> hash_t {
  /** @todo student -  implement the function */
  std::hash<std::string> hasher;
  if constexpr (std::is_same_v<T, std::string>) {
    return hasher(val);
  } else {
    return hasher(std::to_string(val));
  }
}

template <typename T>
auto HyperLogLog<T>::ComputeBinary(const hash_t &hash) const -> std::bitset<MAX_BITS> {
  /** @todo student - implement the function */
  return {hash};
}

template <typename T>
auto HyperLogLog<T>::PositionOfLeftmostOne(const std::bitset<MAX_BITS> &bset) const -> uint64_t {
  /** @todo student - implement the function*/
  const auto start = MAX_BITS - n_bits_ - 1;
  for (int i = start; i >= 0; --i) {
    if (bset.test(i)) {
      return start - i + 1;
    }
  }
  return 0;
}

template <typename T>
auto HyperLogLog<T>::AddElem(T val) -> void {
  /** @todo implement the function */
  if (n_bits_ < 0) {
    return;
  }
  hash_t hash = CalculateHash(val);
  std::bitset<MAX_BITS> binary = ComputeBinary(hash);
  uint64_t position = PositionOfLeftmostOne(binary);
  auto idx = n_bits_ > 0 ? hash >> (MAX_BITS - n_bits_) : 0;
  std::lock_guard<std::mutex> lock(mtx_);
  if (position > max_positions_[idx]) {
    max_positions_[idx] = position;
  }
}

template <typename T>
auto HyperLogLog<T>::ComputeCardinality() -> void {
  /** @todo - student implement the function */
  if (n_bits_ < 0) {
    return;
  }
  double sum = 0;
  for (const auto &pos : max_positions_) {
    sum += std::pow(2, -static_cast<double>(pos));
  }
  const auto m = static_cast<double>(max_positions_.size());
  double alpha_m = CONSTANT * m * m;
  cardinality_ = std::floor(alpha_m / sum);
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
