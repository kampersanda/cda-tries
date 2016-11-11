#include "DacBc.hpp"

namespace cda_tries {

DacBc::DacBc() {}

DacBc::~DacBc() {}

void DacBc::build(const std::vector<bc_t> &bc) {
  clear();
  if (bc.empty()) {
    return;
  }

  std::vector<uint8_t> values[4];
  std::vector<bool> flags[4];
  std::vector<bool> leaf_flags(bc.size());
  std::vector<uint32_t> extras;
  max_level_ = 0;

  values[0].reserve(bc.size() * 2);
  flags[0].reserve(bc.size() * 2);
  extras.reserve(bc.size());

  auto append = [&](uint32_t value, bool is_leaf) {
    if (is_leaf) {
      extras.push_back(value >> 8);
      values[0].push_back(static_cast<uint8_t>(value & 0xFF));
      flags[0].push_back(false);
      return;
    }

    uint32_t level = 0;
    values[level].push_back(static_cast<uint8_t>(value & 0xFF));
    flags[level].push_back(true);
    value >>= 8;

    while (value) {
      ++level;
      values[level].push_back(static_cast<uint8_t>(value & 0xFF));
      flags[level].push_back(true);
      value >>= 8;
    }

    flags[level].back() = false;
    max_level_ = std::max(max_level_, level);
  };

  for (uint32_t i = 0; i < bc.size(); ++i) {
    if (!bc[i].is_fixed()) {
      append(0, false);
      append(0, false);
      ++num_emps_;
    } else {
      auto is_leaf = bc[i].is_leaf();
      leaf_flags[i] = is_leaf;
      is_leaf ? append(bc[i].link(), true) : append(bc[i].base() ^ i, false);
      append(bc[i].check() ^ i, false);
    }
  }

  for (uint32_t j = 0; j < max_level_; ++j) {
    values_[j].build(values[j]);
    flags_[j].build(flags[j]);
  }
  values_[max_level_].build(values[max_level_]);
  leaf_flags_.build(leaf_flags);
  extras_.build(extras);
}

size_t DacBc::size() const {
  return values_[0].size() / 2;
}

size_t DacBc::size_in_bytes() const {
  auto size = dac_size_in_bytes_();
  size += leaf_flags_.size_in_bytes();
  size += extras_.size_in_bytes();
  return size;
}

size_t DacBc::num_emps() const {
  return num_emps_;
}

double DacBc::bytes_per_elem() const {
  return static_cast<double>(size_in_bytes()) / size();
}

void DacBc::stat(std::ostream &os) const {
  os << "Dac BC stat..." << std::endl;
  os << "size          : " << size() << std::endl;
  os << "size in bytes : " << size_in_bytes() << std::endl;
  os << "num_emps      : " << num_emps() << std::endl;
  os << "bytes per elem: " << bytes_per_elem() << std::endl;

  auto size_1st = values_[0].size() - values_[1].size();
  auto size_2nd = values_[1].size() - values_[2].size();
  auto size_3rd = values_[2].size() - values_[3].size();
  auto size_4th = values_[3].size();

  os << "Ratios of each Dac layer..." << std::endl;
  os << "1st: " << static_cast<double>(size_1st) / values_[0].size() << std::endl;
  os << "2nd: " << static_cast<double>(size_2nd) / values_[0].size() << std::endl;
  os << "3rd: " << static_cast<double>(size_3rd) / values_[0].size() << std::endl;
  os << "4th: " << static_cast<double>(size_4th) / values_[0].size() << std::endl;
}

void DacBc::write(std::ostream &os) const {
  os.write(reinterpret_cast<const char *>(&max_level_), sizeof(max_level_));
  for (uint32_t j = 0; j < max_level_; ++j) {
    values_[j].write(os);
    flags_[j].write(os);
  }
  values_[max_level_].write(os);
  leaf_flags_.write(os);
  extras_.write(os);
  os.write(reinterpret_cast<const char *>(&num_emps_), sizeof(num_emps_));
}

void DacBc::read(std::istream &is) {
  clear();
  is.read(reinterpret_cast<char *>(&max_level_), sizeof(max_level_));
  for (uint32_t j = 0; j < max_level_; ++j) {
    values_[j].read(is);
    flags_[j].read(is);
  }
  values_[max_level_].read(is);
  leaf_flags_.read(is);
  extras_.read(is);
  is.read(reinterpret_cast<char *>(&num_emps_), sizeof(num_emps_));
}

void DacBc::clear() {
  for (uint32_t j = 0; j < max_level_; ++j) {
    values_[j].clear();
    flags_[j].clear();
  }
  values_[max_level_].clear();
  leaf_flags_.clear();
  extras_.clear();
  max_level_ = 0;
  num_emps_ = 0;
}

uint32_t DacBc::access_(uint32_t pos) const {
  uint32_t level = 0;
  uint32_t value = values_[level][pos];
  while (flags_[level][pos]) {
    pos = flags_[level].rank(pos);
    ++level;
    value |= static_cast<uint32_t>(values_[level][pos]) << (level * 8);
    if (level == max_level_) {
      break;
    }
  }
  return value;
}

size_t DacBc::dac_size_in_bytes_() const {
  size_t size = 0;
  for (uint32_t j = 0; j < max_level_; ++j) {
    size += values_[j].size_in_bytes();
    size += flags_[j].size_in_bytes();
  }
  size += values_[max_level_].size_in_bytes();
  return size;
}

} // cda_tries
