#include <algorithm>

#include "FlatArray.hpp"

using namespace std;

namespace cda_tries {

void FlatArray::Build(const std::vector<uint> &src) {
  if (src.empty()) {
    return;
  }

  uint maxVal = *max_element(src.begin(), src.end());
  bits_ = 0;

  while (0 < maxVal) {
    bits_++;
    maxVal >>= 1;
  }

  len_ = src.size();
  chunks_.Init(len_ * bits_ / BITS + 1, 0);
  mask_ = static_cast<uint>((1 << bits_) - 1);

  for (size_t i = 0; i < len_; ++i) {
    const uint i_th = i * bits_;
    const uint pos = i_th / BITS;
    const uint offset = i_th % BITS;

    chunks_[pos] &= ~(mask_ << offset);
    chunks_[pos] |= (src[i] & mask_) << offset;
    if (BITS < offset + bits_) {
      chunks_[pos + 1] &= ~(mask_ >> (BITS - offset));
      chunks_[pos + 1] |= (src[i] & mask_) >> (BITS - offset);
    }
  }
}

void FlatArray::Save(std::ostream &os) const {
  chunks_.Save(os);
  os.write(reinterpret_cast<const char *>(&bits_), sizeof(bits_));
  os.write(reinterpret_cast<const char *>(&len_), sizeof(len_));
  os.write(reinterpret_cast<const char *>(&mask_), sizeof(mask_));
}

void FlatArray::Load(std::istream &is) {
  chunks_.Load(is);
  is.read(reinterpret_cast<char *>(&bits_), sizeof(bits_));
  is.read(reinterpret_cast<char *>(&len_), sizeof(len_));
  is.read(reinterpret_cast<char *>(&mask_), sizeof(mask_));
}

} //cda_tries
