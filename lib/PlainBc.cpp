#include "PlainBc.hpp"

namespace cda_tries {

void PlainBc::build(const std::vector<bc_t> &bc) {
  clear();
  if (bc.empty()) {
    return;
  }

  bc_.reset(bc.size());
  for (size_t i = 0; i < bc.size(); ++i) {
    bc_[i] = bc[i];
    if (!bc_[i].is_fixed()) {
      ++num_emps_;
    }
  }
}

size_t PlainBc::size() const {
  return bc_.size();
}

size_t PlainBc::size_in_bytes() const {
  return bc_.size_in_bytes();
}

size_t PlainBc::num_emps() const {
  return num_emps_;
}

double PlainBc::bytes_per_elem() const {
  return sizeof(bc_t);
}

void PlainBc::stat(std::ostream &os) const {
  os << "Plain BC stat..." << std::endl;
  os << "size          : " << size() << std::endl;
  os << "size in bytes : " << size_in_bytes() << std::endl;
  os << "num_emps      : " << num_emps() << std::endl;
  os << "bytes per elem: " << sizeof(bc_t) << std::endl;
}

void PlainBc::write(std::ostream &os) const {
  bc_.write(os);
  os.write(reinterpret_cast<const char *>(&num_emps_), sizeof(num_emps_));
}

void PlainBc::read(std::istream &is) {
  clear();
  bc_.read(is);
  is.read(reinterpret_cast<char *>(&num_emps_), sizeof(num_emps_));
}

void PlainBc::clear() {
  bc_.clear();
  num_emps_ = 0;
}

} // cda_tries
