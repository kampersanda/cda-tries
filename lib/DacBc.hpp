#ifndef CDA_TRIES_DAC_BC_HPP
#define CDA_TRIES_DAC_BC_HPP

#include "Bc.hpp"
#include "BitArray.hpp"
#include "SmallArray.hpp"

namespace cda_tries {

class DacBc : public Bc {
public:
  DacBc();
  ~DacBc();

  void build(const std::vector<bc_t> &bc);

  uint32_t base(uint32_t pos) const {
    return access_(pos * 2) ^ pos;
  }
  uint32_t link(uint32_t pos) const {
    return values_[0][pos * 2] | (extras_[leaf_flags_.rank(pos)] << 8);
  }
  uint32_t check(uint32_t pos) const {
    return access_(pos * 2 + 1) ^ pos;
  }
  bool is_leaf(uint32_t pos) const {
    return leaf_flags_[pos];
  }
  bool is_fixed(uint32_t pos) const {
    return check(pos) != pos;
  }

  size_t size() const;
  size_t size_in_bytes() const;
  size_t num_emps() const;
  double bytes_per_elem() const;
  void status(std::ostream &os) const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  DacBc(const DacBc &) = delete;
  DacBc &operator=(const DacBc &) = delete;

private:
  Array<uint8_t> values_[4];
  BitArray flags_[3];
  BitArray leaf_flags_;
  SmallArray extras_;
  uint32_t max_level_ = 0;
  size_t num_emps_ = 0;

  uint32_t access_(uint32_t pos) const;
  size_t dac_size_in_bytes_() const;
};

} // cda_tries

#endif // CDA_TRIES_DAC_BC_HPP
