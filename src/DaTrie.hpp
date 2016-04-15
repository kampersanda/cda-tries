#ifndef CDA_TRIES_TRIE_HPP
#define CDA_TRIES_TRIE_HPP

#include <limits>

#include "BitArray.hpp"
#include "BC.hpp"
#include "CodeTable.hpp"

namespace cda_tries {

class DaTrie {
public:
  static const uint NOT_FOUND = std::numeric_limits<uint32_t>::max();

  DaTrie() {}
  DaTrie(std::istream &is, bc_e type);
  DaTrie(const std::vector<std::string> &strs, bc_e type);

  ~DaTrie() {}

  uint Lookup(const char *str) const;
  void Access(uint id, std::string &str) const;

  size_t NumElems() const;
  size_t AllocSize() const;

  void Stat(std::ostream &os) const;
  void Save(std::ostream &os) const;

  DaTrie(const DaTrie &) = delete;
  DaTrie &operator=(const DaTrie &) = delete;

private:
  size_t numStrs_ = 0;
  size_t maxLen_ = 0;

  CodeTable table_;
  std::unique_ptr<BC> bc_;
  BitArray terms_;
  Array<char> tail_;

  uint IdxToId(uint idx) const { return terms_.Rank(idx); };
  uint IdToIdx(uint id) const { return terms_.Select(id); };
};

} //cda_tries

#endif //CDA_TRIES_TRIE_HPP
