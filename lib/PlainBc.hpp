#ifndef CDA_TRIES_PLAIN_BC_HPP
#define CDA_TRIES_PLAIN_BC_HPP

#include "Array.hpp"
#include "Bc.hpp"

namespace cda_tries {

class PlainBc : public Bc {
public:
  PlainBc() {}
  ~PlainBc() {}

  void build(const std::vector<bc_t> &bc);

  uint32_t base(uint32_t pos) const {
    return bc_[pos].base();
  }
  uint32_t link(uint32_t pos) const {
    return bc_[pos].link();
  }
  uint32_t check(uint32_t pos) const {
    return bc_[pos].check();
  }
  bool is_leaf(uint32_t pos) const {
    return bc_[pos].is_leaf();
  }
  bool is_fixed(uint32_t pos) const {
    return bc_[pos].is_fixed();
  }

  size_t size() const;;
  size_t size_in_bytes() const;
  size_t num_emps() const;
  double bytes_per_elem() const;
  void status(std::ostream &os) const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  PlainBc(const PlainBc &) = delete;
  PlainBc &operator=(const PlainBc &) = delete;

private:
  Array<bc_t> bc_;
  size_t num_emps_ = 0;
};

} // cda_tries

#endif // CDA_TRIES_PLAIN_BC_HPP
