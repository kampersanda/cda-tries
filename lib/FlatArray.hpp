#ifndef CDA_TRIES_FLATARRAY_HPP
#define CDA_TRIES_FLATARRAY_HPP

#include "Array.hpp"

namespace cda_tries {

class FlatArray {
public:
  FlatArray() {}

  void Build(const std::vector<uint> &src);

  uint operator[](uint idx) const {
    const uint i_th = idx * bits_;
    const uint pos = i_th / BITS;
    const uint offset = i_th % BITS;

    if (offset + bits_ <= BITS) {
      return (chunks_[pos] >> offset) & mask_;
    } else {
      return ((chunks_[pos] >> offset) | (chunks_[pos + 1] << (BITS - offset))) & mask_;
    }
  }

  bool IsEmpty() const { return len_ == 0; }
  size_t BitsPerElem() const { return bits_; }
  size_t Length() const { return len_; }
  size_t AllocSize() const { return chunks_.AllocSize(); }

  void Save(std::ostream &os) const;
  void Load(std::istream &is);

  FlatArray(const FlatArray &) = delete;
  FlatArray &operator=(const FlatArray &) = delete;

private:
  const uint BITS = sizeof(uint) * 8;
  Array<uint> chunks_;
  size_t bits_ = 0;
  size_t len_ = 0;
  uint mask_ = 0;
};

} //cda_tries

#endif //CDA_TRIES_FLATARRAY_HPP
