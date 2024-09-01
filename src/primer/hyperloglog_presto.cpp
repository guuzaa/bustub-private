#include "primer/hyperloglog_presto.h"
#include <cmath>
#include <iostream>

namespace bustub {

template <typename T>
auto HyperLogLogPresto<T>::AddElem(T val) -> void {
  if (n_leading_bits_ < 0) {
    return;
  }

  auto hash = CalculateHash(val);
  uint16_t bucket_idx = n_leading_bits_ > 0 ? hash >> (64 - n_leading_bits_) : 0;
  uint64_t trailing_zeros = PositionOfRightmostOne(hash);
  const auto lsb = trailing_zeros & 0xF;
  const auto msb = trailing_zeros >> BUCKET_SIZE;
  dense_bucket_[bucket_idx] =
      dense_bucket_[bucket_idx].to_ulong() > lsb ? dense_bucket_[bucket_idx] : std::bitset<BUCKET_SIZE>(lsb);

  if (msb > 0) {
    uint64_t raw = overflow_bucket_.count(bucket_idx) > 0 ? overflow_bucket_[bucket_idx].to_ulong() : 0;
    overflow_bucket_[bucket_idx] = std::max(raw, msb);
  }
}

template <typename T>
auto HyperLogLogPresto<T>::PositionOfRightmostOne(hash_t hash) const -> uint64_t {
  uint64_t max = sizeof(hash_t) * 8 - n_leading_bits_;
  if (hash == 0) {  // All bits are zero
    return max;
  }
  return std::min(static_cast<uint64_t>(__builtin_ctzll(hash)), max);
}

template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  if (n_leading_bits_ < 0) {
    return;
  }

  double sum = 0.0;
  const uint64_t num_buckets = dense_bucket_.size();
  for (uint64_t i = 0; i < num_buckets; ++i) {
    uint8_t value = dense_bucket_[i].to_ulong();
    if (value != 0 && overflow_bucket_.count(i) > 0) {
      value += overflow_bucket_[i].to_ulong() << BUCKET_SIZE;
    }
    sum += std::pow(2.0, -static_cast<double>(value));
  }

  double alpha_m = CONSTANT * num_buckets * num_buckets;
  cardinality_ = static_cast<uint64_t>(std::floor(alpha_m / sum));
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub
