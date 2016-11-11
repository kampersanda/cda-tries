#include "FastDacBc.hpp"

namespace cda_tries {

FastDacBc::FastDacBc() {}

FastDacBc::~FastDacBc() {}

void FastDacBc::build(const std::vector<bc_t> &bc) {
  clear();
  if (bc.empty()) {
    return;
  }

  std::vector<uint8_t>  values_1st;
  std::vector<uint16_t> values_2nd;
  std::vector<uint32_t> values_3rd;
  std::vector<uint32_t> ranks[2];
  std::vector<bool> leaf_flags(bc.size());
  std::vector<uint32_t> extras;

  values_1st.reserve(bc.size() * 2);
  ranks[0].reserve((bc.size() * 2) / 128);

  auto append = [&](uint32_t value, bool is_leaf) {
    if ((values_1st.size() % 128) == 0) {
      ranks[0].push_back(static_cast<uint32_t>(values_2nd.size()));
    }

    if (is_leaf) {
      values_1st.push_back(static_cast<uint8_t>(value & 0xFF));
      extras.push_back(value >> 8);
      return;
    }

    if ((value / 128) == 0) {
      values_1st.push_back(static_cast<uint8_t>(0 | (value << 1)));
      return;
    } else {
      auto pos = values_2nd.size() - ranks[0].back();
      values_1st.push_back(static_cast<uint8_t>(1 | (pos << 1)));
    }

    if ((values_2nd.size() % 32768) == 0) {
      ranks[1].push_back(static_cast<uint32_t>(values_3rd.size()));
    }

    if ((value / 32768) == 0) {
      values_2nd.push_back(static_cast<uint16_t>(0 | (value << 1)));
      return;
    } else {
      auto pos = values_3rd.size() - ranks[1].back();
      values_2nd.push_back(static_cast<uint16_t>(1 | (pos << 1)));
    }

    values_3rd.push_back(value);
  };


  for (uint32_t i = 0; i < bc.size(); ++i) {
    if (!bc[i].is_fixed()) {
      append(0, false);
      append(0, false);
      ++num_emps_;
    } else {
      bool is_leaf = bc[i].is_leaf();
      leaf_flags[i] = is_leaf;
      is_leaf ? append(bc[i].link(), true) : append(bc[i].base() ^ i, false);
      append(bc[i].check() ^ i, false);
    }
  }

  values_1st_.build(values_1st);
  values_2nd_.build(values_2nd);
  values_3rd_.build(values_3rd);
  ranks_[0].build(ranks[0]);
  ranks_[1].build(ranks[1]);
  leaf_flags_.build(leaf_flags);
  extras_.build(extras);
}

size_t FastDacBc::size() const {
  return values_1st_.size() / 2;
}

size_t FastDacBc::size_in_bytes() const {
  auto size = dac_size_in_bytes_();
  size += leaf_flags_.size_in_bytes();
  size += extras_.size_in_bytes();
  return size;
}

size_t FastDacBc::num_emps() const {
  return num_emps_;
}

double FastDacBc::bytes_per_elem() const {
  return static_cast<double>(size_in_bytes()) / size();
}

void FastDacBc::stat(std::ostream &os) const {
  os << "Fast Dac BC stat..." << std::endl;
  os << "size          : " << size() << std::endl;
  os << "size in bytes : " << size_in_bytes() << std::endl;
  os << "num_emps      : " << num_emps() << std::endl;
  os << "bytes per elem: " << bytes_per_elem() << std::endl;

  auto size_1st = values_1st_.size() - values_2nd_.size();
  auto size_2nd = values_2nd_.size() - values_3rd_.size();
  auto size_3rd = values_3rd_.size();

  os << "Ratios of each Dac layer..." << std::endl;
  os << "1st: " << static_cast<double>(size_1st) / values_1st_.size() << std::endl;
  os << "2nd: " << static_cast<double>(size_2nd) / values_1st_.size() << std::endl;
  os << "3rd: " << static_cast<double>(size_3rd) / values_1st_.size() << std::endl;
}

void FastDacBc::write(std::ostream &os) const {
  values_1st_.write(os);
  values_2nd_.write(os);
  values_3rd_.write(os);
  ranks_[0].write(os);
  ranks_[1].write(os);
  leaf_flags_.write(os);
  extras_.write(os);
  os.write(reinterpret_cast<const char *>(&num_emps_), sizeof(num_emps_));
}

void FastDacBc::read(std::istream &is) {
  clear();
  values_1st_.read(is);
  values_2nd_.read(is);
  values_3rd_.read(is);
  ranks_[0].read(is);
  ranks_[1].read(is);
  leaf_flags_.read(is);
  extras_.read(is);
  is.read(reinterpret_cast<char *>(&num_emps_), sizeof(num_emps_));
}

void FastDacBc::clear() {
  values_1st_.clear();
  values_2nd_.clear();
  values_3rd_.clear();
  ranks_[0].clear();
  ranks_[1].clear();
  leaf_flags_.clear();
  extras_.clear();
  num_emps_ = 0;
}

uint32_t FastDacBc::access_(uint32_t pos) const {
  uint32_t value = values_1st_[pos] >> 1;
  if ((values_1st_[pos] & 1U) == 0) {
    return value;
  }
  pos = ranks_[0][pos / 128] + value;
  value = values_2nd_[pos] >> 1;
  if ((values_2nd_[pos] & 1U) == 0) {
    return value;
  }
  pos = ranks_[1][pos / 32768] + value;
  return values_3rd_[pos];
}

size_t FastDacBc::dac_size_in_bytes_() const {
  size_t size = 0;
  size += values_1st_.size_in_bytes();
  size += values_2nd_.size_in_bytes();
  size += values_3rd_.size_in_bytes();
  size += ranks_[0].size_in_bytes();
  size += ranks_[1].size_in_bytes();
  return size;
}

} // cda_tries
