#include "primer/hyperloglog.h"

namespace bustub {

template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) : cardinality_(0), n_bits_(n_bits) {
  if (n_bits_ < 0 || n_bits_ >= BITSET_CAPACITY) {
    return;
  }
  registers_.resize(1ULL << n_bits, 0);
}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  /** @TODO(student) Implement this function! */
  return {hash};
}

template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  /** @TODO(student) Implement this function! */
  const auto start = BITSET_CAPACITY - n_bits_ - 1;
  for (int16_t i = start; i >= 0; i--) {
    if (bset.test(i)) {
      return start - i + 1;
    }
  }
  return 0;
}

template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  if (n_bits_ < 0) {
    return;
  }
  auto hash = CalculateHash(val);
  std::bitset<BITSET_CAPACITY> binary = ComputeBinary(hash);
  auto id = n_bits_ > 0 ? hash >> (BITSET_CAPACITY - n_bits_) : 0;
  auto pos = PositionOfLeftmostOne(binary);
  std::lock_guard<std::mutex> lock(mtx_);
  if (registers_[id] < pos) {
    registers_[id] = pos;
  }
}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  /** @TODO(student) Implement this function! */
  if (n_bits_ < 0) {
    return;
  }
  double sum = 0;
  for (const auto reg : registers_) {
    sum += std::pow(2.0, -static_cast<double>(reg));
  }
  const auto m = static_cast<double>(registers_.size());
  double alpha_m = m * m * CONSTANT;
  cardinality_ = static_cast<size_t>(std::floor(alpha_m / sum));
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
