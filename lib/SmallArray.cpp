#include <algorithm>

#include "SmallArray.hpp"

namespace cda_tries {

SmallArray::SmallArray() {}

SmallArray::~SmallArray() {}

void SmallArray::build(const std::vector<uint32_t> &array) {
  clear();
  if (array.empty()) {
    return;
  }

  bits_ = 0;
  auto max_value = *std::max_element(array.begin(), array.end());
  do {
    ++bits_;
    max_value >>= 1;
  } while (0 < max_value);

  size_ = array.size();
  chunks_.reset(size_ * bits_ / 32 + 1, 0);
  mask_ = static_cast<uint32_t>((1 << bits_) - 1);

  for (uint32_t i = 0; i < size_; ++i) {
    auto chunk_pos = i * bits_ / 32;
    auto offset = i * bits_ % 32;
    chunks_[chunk_pos] &= ~(mask_ << offset);
    chunks_[chunk_pos] |= (array[i] & mask_) << offset;
    if (32 < offset + bits_) {
      chunks_[chunk_pos + 1] &= ~(mask_ >> (32 - offset));
      chunks_[chunk_pos + 1] |= (array[i] & mask_) >> (32 - offset);
    }
  }
}

size_t SmallArray::size() const {
  return size_;
}

size_t SmallArray::size_in_bytes() const {
  return chunks_.size_in_bytes();
}

size_t SmallArray::bits_per_elem() const {
  return bits_;
}

void SmallArray::write(std::ostream &os) const {
  chunks_.write(os);
  os.write(reinterpret_cast<const char *>(&size_), sizeof(size_));
  os.write(reinterpret_cast<const char *>(&bits_), sizeof(bits_));
  os.write(reinterpret_cast<const char *>(&mask_), sizeof(mask_));
}

void SmallArray::read(std::istream &is) {
  clear();
  chunks_.read(is);
  is.read(reinterpret_cast<char *>(&size_), sizeof(size_));
  is.read(reinterpret_cast<char *>(&bits_), sizeof(bits_));
  is.read(reinterpret_cast<char *>(&mask_), sizeof(mask_));
}

void SmallArray::clear() {
  chunks_.clear();
  size_ = 0;
  bits_ = 0;
  mask_ = 0;
}

} // cda_tries
