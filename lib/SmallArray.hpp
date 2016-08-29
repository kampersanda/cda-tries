#ifndef CDA_TRIES_SMALL_ARRAY_HPP
#define CDA_TRIES_SMALL_ARRAY_HPP

#include "Array.hpp"

namespace cda_tries {

class SmallArray {
public:
  SmallArray();
  ~SmallArray();

  void build(const std::vector<uint32_t> &array);

  uint32_t operator[](uint32_t pos) const {
    auto chunk_pos = pos * bits_ / 32;
    auto offset = pos * bits_ % 32;
    if (offset + bits_ <= 32) {
      return (chunks_[chunk_pos] >> offset) & mask_;
    } else {
      return ((chunks_[chunk_pos] >> offset) | (chunks_[chunk_pos + 1] << (32 - offset))) & mask_;
    }
  }

  size_t size() const;
  size_t size_in_bytes() const;
  size_t bits_per_elem() const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  SmallArray(const SmallArray &) = delete;
  SmallArray &operator=(const SmallArray &) = delete;

private:
  Array<uint32_t> chunks_;
  size_t   size_ = 0;
  uint32_t bits_ = 0;
  uint32_t mask_ = 0;
};

} // cda_tries

#endif // CDA_TRIES_SMALL_ARRAY_HPP
