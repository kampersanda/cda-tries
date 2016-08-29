#include <array>
#include <algorithm>

#include "CodeTable.hpp"

namespace cda_tries {

CodeTable::CodeTable() { clear(); }

CodeTable::~CodeTable() {}

struct freq_t {
  size_t freq   = 0;
  uint8_t label = 0;
  bool operator<(const freq_t &rhs) const {
    return freq > rhs.freq;
  }
};

size_t CodeTable::build(const std::vector<std::string> &strs) {
  if (strs.empty()) {
    return 0;
  }

  std::array<freq_t, 256> freqs;

  for (uint8_t label = 0;; ++label) {
    freqs[label].label = label;
    if (label == UINT8_MAX) {
      break;
    }
  }

  size_t max_length = 0;
  for (auto &str : strs) {
    for (uint8_t label : str) {
      ++freqs[label].freq;
    }
    max_length = std::max(max_length, str.size());
  }

  std::sort(freqs.begin(), freqs.end());

  for (uint8_t label = 0;; ++label) {
    table_[freqs[label].label] = label;
    if (label == UINT8_MAX) {
      break;
    }
  }

  for (uint8_t label = 0;; ++label) {
    table_[table_[label] + 256] = label;
    if (label == UINT8_MAX) {
      break;
    }
  }

  return max_length;
}

size_t CodeTable::size() const {
  return 512;
}

size_t CodeTable::size_in_bytes() const {
  return sizeof(table_);
}

void CodeTable::write(std::ostream &os) const {
  os.write(reinterpret_cast<const char *>(&table_[0]), sizeof(table_));
}

void CodeTable::read(std::istream &is) {
  is.read(reinterpret_cast<char *>(&table_[0]), sizeof(table_));
}

void CodeTable::clear() {
  for (uint8_t label = 0;; ++label) {
    table_[label] = label;
    table_[label + 256] = label;
    if (label == UINT8_MAX) {
      break;
    }
  }
}

} // cda_tries
