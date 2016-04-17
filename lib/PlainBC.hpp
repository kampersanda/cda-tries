#ifndef CDA_TRIES_PLAINBC_HPP
#define CDA_TRIES_PLAINBC_HPP

#include "Array.hpp"
#include "BC.hpp"

namespace cda_tries {

class PlainBC : public BC {
public:
  PlainBC(std::istream &is)  { bc_.Load(is); }
  PlainBC(const std::vector<bc_t> &bc, const std::vector<bool> &leafs);

  uint Base(uint idx) const { return bc_[idx].base >> 1; }
  uint Check(uint idx) const { return bc_[idx].check; }
  uint Link(uint idx) const { return Base(idx); }
  bool IsLeaf(uint idx) const { return (bc_[idx].base & 1) == 1; }

  size_t Length() const { return bc_.Length(); }
  size_t AllocSize() const { return bc_.AllocSize(); }
  void Stat(std::ostream &os) const;
  void Save(std::ostream &os) const  { bc_.Save(os); }

  PlainBC(const PlainBC &) = delete;
  PlainBC &operator=(const PlainBC &) = delete;

private:
  Array<bc_t> bc_;
};

} //cda_tries

#endif //CDA_TRIES_PLAINBC_HPP
