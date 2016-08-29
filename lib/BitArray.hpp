#ifndef CDA_TRIES_BIT_ARRAY_HPP
#define CDA_TRIES_BIT_ARRAY_HPP

#include "Array.hpp"

namespace cda_tries {

class BitArray {
public:
  static constexpr uint32_t R1_SIZE   = 256;
  static constexpr uint32_t R2_SIZE   = 32;
  static constexpr uint32_t R1_PER_R2 = R1_SIZE / R2_SIZE;

  BitArray();
  ~BitArray();

  void build(const std::vector<bool> &bits);

  bool operator[](uint32_t pos) const {
    return (bits_[pos / 32] & (1U << (pos % 32))) != 0;
  }

  uint32_t rank(uint32_t pos) const; // # of 1s in B[0,pos)
  uint32_t select(uint32_t count) const; // pos of the count+1 th occurrence

  size_t size() const;
  size_t size_in_bytes() const;

  size_t num_1s() const;
  size_t num_0s() const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  BitArray(const BitArray &) = delete;
  BitArray &operator=(const BitArray &) = delete;

private:
  struct block_t {
    uint32_t rank_1st = 0;
    uint8_t  rank_2nd[R1_PER_R2] = {};
  };
  Array<uint32_t> bits_;
  Array<block_t>  blocks_;
  size_t size_   = 0;
  size_t num_1s_ = 0;
};

} // cda_tries

#endif // CDA_TRIES_BIT_ARRAY_HPP
