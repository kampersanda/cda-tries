#ifndef CDA_TRIES_DACBC_HPP
#define CDA_TRIES_DACBC_HPP

#include "BitArray.hpp"
#include "BC.hpp"
#include "FlatArray.hpp"

namespace cda_tries {

class DacBC : public BC {
public:
  DacBC(std::istream &is);
  DacBC(const std::vector<bc_t> &bc, const std::vector<bool> &leafs);

  ~DacBC() {}

  uint Base(uint idx) const { return Access(idx * 2) ^ idx; }
  uint Check(uint idx) const { return Access(idx * 2 + 1) ^ idx; }
  uint Link(uint idx) const { return Byte(idx * 2) | (links_[leafs_.Rank(idx)] << 8); }
  bool IsLeaf(uint idx) const { return leafs_[idx]; }

  size_t Length() const;
  size_t AllocSize() const;
  void Stat(std::ostream &os) const;
  void Save(std::ostream &os) const;

  DacBC(const DacBC &) = delete;
  DacBC &operator=(const DacBC &) = delete;

private:
  uint numLevels_;
  Array<uint8_t> bytes_[4];
  BitArray bits_[3];
  BitArray leafs_;
  FlatArray links_;

  uint Access(uint pos) const {
    uint level = 0;
    uint val = bytes_[level][pos];
    while (bits_[level][pos]) {
      pos = bits_[level].Rank(pos);
      level++;
      val |= static_cast<uint>(bytes_[level][pos]) << (level * 8);
      if (level + 1 == numLevels_) {
        break;
      }
    }
    return val;
  }

  uint8_t Byte(const uint pos) const { return bytes_[0][pos]; }
  size_t DacAllocSize() const;
};

} //cda_tries

#endif //CDA_TRIES_DACBC_HPP
