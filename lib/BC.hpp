#ifndef CDA_TRIES_BC_HPP
#define CDA_TRIES_BC_HPP


#include <memory>
#include <vector>

#include "Basic.hpp"

namespace cda_tries {

class BC {
public:
  static std::unique_ptr<BC> Load(std::istream &is, bc_e type);
  static std::unique_ptr<BC> Build(const std::vector<bc_t> &bc,
                                   const std::vector<bool> &leafs, bc_e type);

  virtual ~BC() {}

  virtual uint Base(uint idx) const = 0;
  virtual uint Check(uint idx) const = 0;
  virtual uint Link(uint idx) const = 0;
  virtual bool IsLeaf(uint idx) const = 0;

  virtual size_t Length() const = 0;
  virtual size_t AllocSize() const = 0;
  virtual void Stat(std::ostream &os) const = 0;
  virtual void Save(std::ostream &os) const = 0;
};

} //cda_tries

#endif //CDA_TRIES_BC_HPP
