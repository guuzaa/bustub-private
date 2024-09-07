#include "primer/hyperloglog_presto.h"

namespace bustub {

template <typename KeyType>
HyperLogLogPresto<KeyType>::HyperLogLogPresto(int16_t n_leading_bits) : cardinality_(0), n_bits_(n_leading_bits) {
  if (n_leading_bits < 0) {
    return;
  }
  dense_bucket_.resize(1ULL << n_bits_, 0);
}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  if (n_bits_ < 0) {
    return;
  }
  auto hash = CalculateHash(val);
  auto binary = ComputeBinary(hash);
  auto trailing_zeros = CountRightMostZeros(binary);
  uint16_t index = n_bits_ == 0 ? 0 : hash >> (sizeof(hash_t) * 8 - n_bits_);
  auto max_trailing_zeros = dense_bucket_[index].to_ullong();
  if (overflow_bucket_.count(index) > 0) {
    max_trailing_zeros += overflow_bucket_[index].to_ullong() << DENSE_BUCKET_SIZE;
  }

  if (max_trailing_zeros >= trailing_zeros) {
    return;
  }

  const auto lsb = trailing_zeros & 0xF;
  const auto msb = trailing_zeros >> DENSE_BUCKET_SIZE;
  dense_bucket_[index] = std::bitset<DENSE_BUCKET_SIZE>(lsb);
  overflow_bucket_[index] = std::bitset<OVERFLOW_BUCKET_SIZE>(msb);
}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::CountRightMostZeros(const std::bitset<BITSET_CAPACITY> &binary) -> uint64_t {
  const uint64_t max = sizeof(hash_t) * 8 - n_bits_;
  for (uint64_t i = 0; i < max; i++) {
    if (binary.test(i)) {
      return i;
    }
  }
  return max;
}

template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  /** @TODO(student) Implement this function! */
  if (n_bits_ < 0) {
    return;
  }
  double sum = 0;
  const auto m = static_cast<double>(dense_bucket_.size());
  for (uint64_t i = 0; i < m; i++) {
    uint64_t value = dense_bucket_[i].to_ullong();
    if (overflow_bucket_.count(i) > 0) {
      value += overflow_bucket_[i].to_ullong() << DENSE_BUCKET_SIZE;
    }
    sum += std::pow(2.0, -static_cast<double>(value));
  }
  double alpha_m = m * m * CONSTANT;
  cardinality_ = static_cast<size_t>(std::floor(alpha_m / sum));
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub
