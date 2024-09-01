#pragma once

#include <bitset>
#include <functional>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <vector>

#define MAX_BITS 64
#define DEFAULT_CARDINALITY 0

namespace bustub {

/** @brief Hash value type. */
using hash_t = uint64_t;

/** @brief Constant for HLL. */
static const double CONSTANT = 0.79402;

template <typename T>
class HyperLogLog {
 public:
  HyperLogLog() = delete;

  explicit HyperLogLog(int16_t n_bits) : cardinality_(0) {
    if (n_bits < 0) {
      return;
    }
    max_positions_.resize(1ULL << static_cast<uint64_t>(n_bits), 0);
    n_bits_ = n_bits;
  }

  /**
   * @brief Getter value for cardinality.
   *
   * @returns cardinality value
   */
  auto GetCardinality() { return cardinality_; }

  /**
   * @brief Ddds a value into the HyperLogLog.
   *
   * @param[in] val - value that's added into hyperloglog
   */
  auto AddElem(T val) -> void;

  /**
   * @brief Function that computes cardinality.
   */
  auto ComputeCardinality() -> void;

 private:
  /**
   * @brief Calculates Hash of a given value.
   *
   * @param[in] val - value
   * @returns hash integer of given input value
   */
  inline auto CalculateHash(T val) -> hash_t;

  /**
   * @brief Function that computes binary.
   *
   *
   * @param[in] hash
   * @returns binary of a given hash
   */
  auto ComputeBinary(const hash_t &hash) const -> std::bitset<MAX_BITS>;

  /**
   * @brief Function that computes leading zeros.
   *
   * @param[in] bset - binary values of a given bitset
   * @returns leading zeros of given binary set
   */
  auto PositionOfLeftmostOne(const std::bitset<MAX_BITS> &bset) const -> uint64_t;

  /** @brief Cardinality value. */
  size_t cardinality_;

  /** @todo (student) can add their data structures that support HyperLogLog */
  std::vector<uint64_t> max_positions_;
  int16_t n_bits_;
  std::mutex mtx_;  // for max_positions_
};

}  // namespace bustub
