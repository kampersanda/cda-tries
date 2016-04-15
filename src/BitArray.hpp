#ifndef CDA_TRIES_BITARRAY_HPP
#define CDA_TRIES_BITARRAY_HPP

#include "Array.hpp"

namespace cda_tries {

class BitArray {
public:
  BitArray() {}
  ~BitArray() {}

  void Build(const std::vector<bool> &bits);

  bool operator[](uint idx) const {
    return (bits_[idx / 32] & (1U << (idx % 32))) != 0;
  }

  uint Rank(uint idx) const {
    return rank1st_[idx / R1_BLK_LEN] + rank2nd_[idx / R2_BLK_LEN] +
      PopCount((bits_[idx / 32] & ((1U << (idx % 32)) - 1)));
  }
  uint Select(uint cnt) const;

  size_t NumBits() const;
  size_t Num1s() const;
  size_t AllocSize() const;

  void Save(std::ostream &os) const;
  void Load(std::istream &is);

  BitArray(const BitArray &) = delete;
  BitArray &operator=(const BitArray &) = delete;

private:
  const uint R1_BLK_LEN = 1U << 8;
  const uint R2_BLK_LEN = 1U << 5;
  const uint BLKS_PER_1ST = R1_BLK_LEN / R2_BLK_LEN;

  Array<uint32_t> bits_;
  Array<uint32_t> rank1st_;
  Array<uint8_t> rank2nd_;
  size_t numBits_ = 0;
  size_t num1s_ = 0;

  uint PopCount(uint val) const {
    val = ((val & 0xAAAAAAAA) >> 1) + (val & 0x55555555);
    val = ((val & 0xCCCCCCCC) >> 2) + (val & 0x33333333);
    val = ((val >> 4) + val) & 0x0F0F0F0F;
    val = (val >> 8) + val;
    return ((val >> 16) + val) & 0x3F;
  }
};

} //cda_tries

#endif //CDA_TRIES_BITARRAY_HPP
