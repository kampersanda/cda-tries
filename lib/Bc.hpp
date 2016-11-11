#ifndef CDA_TRIES_BC_HPP
#define CDA_TRIES_BC_HPP

#include "Basic.hpp"

namespace cda_tries {

class Bc {
public:
  static std::unique_ptr<Bc> create(bc_type type);
  virtual ~Bc();

  virtual void build(const std::vector<bc_t> &bc) = 0;

  virtual uint32_t base(uint32_t pos) const = 0;
  virtual uint32_t link(uint32_t pos) const = 0;
  virtual uint32_t check(uint32_t pos) const = 0;
  virtual bool is_leaf(uint32_t pos) const = 0;
  virtual bool is_fixed(uint32_t pos) const = 0;

  virtual size_t size() const = 0;
  virtual size_t size_in_bytes() const = 0;
  virtual size_t num_emps() const = 0;
  virtual double bytes_per_elem() const = 0;
  virtual void stat(std::ostream &os) const = 0;

  virtual void write(std::ostream &os) const = 0;
  virtual void read(std::istream &is) = 0;

  virtual void clear() = 0;
};

} // cda_tries

#endif // CDA_TRIES_BC_HPP
