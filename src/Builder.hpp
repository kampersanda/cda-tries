#ifndef CDA_TRIES_TRIEBUILDER_HPP
#define CDA_TRIES_TRIEBUILDER_HPP

#include <limits>

#include "CodeTable.hpp"

namespace cda_tries {

class Builder {
public:
  friend class DaTrie;

  Builder(const std::vector<std::string> &strs, const CodeTable &table, bc_e type);
  ~Builder() {}

  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;

private:
  struct suffix_t {
    std::string rstr;
    uint idx;
  };

  static const uint INV_IDX = std::numeric_limits<uint32_t>::max();
  static const size_t UPPER = std::numeric_limits<uint32_t>::max() >> 1;

  const std::vector<std::string> &strs_;
  const CodeTable &table_;
  const bc_e type_;

  std::vector<bc_t> bc_;
  std::vector<bool> leafs_;
  std::vector<bool> terms_;
  std::vector<char> tail_;

  std::vector<bool> fixed_;
  std::vector<uint> heads_; // for each block
  std::vector<uint8_t> edges_;
  std::vector<suffix_t> suffixes_;

  uint blkLen_;
  uint emptyHead_;

  void Expand();
  void Fix(uint idx);
  void Arrange(size_t begin, size_t end, size_t level, uint idx);
  void DeleteEmptyList();
  void UnifyTail();

  uint XCheck() const;
  uint YCheck(uint blk) const;
  bool IsTarget(uint base) const;

  uint Prev(uint idx) const { return bc_[idx].base; }
  uint Next(uint idx) const { return bc_[idx].check; }
  void SetPrev(uint idx, uint prev) { bc_[idx].base = prev; }
  void SetNext(uint idx, uint next) { bc_[idx].check = next; }
};

} //cda_tries

#endif //CDA_TRIES_TRIEBUILDER_HPP
