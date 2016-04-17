#ifndef CDA_TRIES_FASTDACBC_HPP
#define CDA_TRIES_FASTDACBC_HPP

#include "BitArray.hpp"
#include "BC.hpp"
#include "FlatArray.hpp"

namespace cda_tries {

class FastDacBC : public BC {
public:
  FastDacBC(std::istream &is);
  FastDacBC(const std::vector<bc_t> &bc, const std::vector<bool> &leafs);

  uint Base(uint idx) const { return Access(idx * 2) ^ idx; }
  uint Check(uint idx) const { return Access(idx * 2 + 1) ^ idx; }
  uint Link(uint idx) const { return Byte(idx * 2) | (links_[leafs_.Rank(idx)] << 8); }
  bool IsLeaf(uint idx) const { return leafs_[idx]; }

  size_t Length() const;
  size_t AllocSize() const;
  void Stat(std::ostream &os) const;
  void Save(std::ostream &os) const;

  FastDacBC(const FastDacBC &) = delete;
  FastDacBC &operator=(const FastDacBC &) = delete;

private:
  static const uint BLK1_LEN = 1U << 7;
  static const uint BLK2_LEN = 1U << 15;

  Array<uint8_t> first_;
  Array<uint16_t> second_;
  Array<uint32_t> last_;
  Array<uint32_t> ranks_[2];
  BitArray leafs_;
  FlatArray links_;

  uint Access(uint pos) const {
    uint val = first_[pos] >> 1;
    if ((first_[pos] & 1U) == 0) {
      return val;
    }
    pos = ranks_[0][pos / BLK1_LEN] + val;
    val = second_[pos] >> 1;
    if ((second_[pos] & 1U) == 0) {
      return val;
    }
    pos = ranks_[1][pos / BLK2_LEN] + val;
    return last_[pos];
  }

  uint8_t Byte(const uint pos) const { return first_[pos]; }
  size_t DacAllocSize() const;
};

} //cda_tries

#endif //CDA_TRIES_FASTDACBC_HPP
