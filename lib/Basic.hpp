//
// Created by Shunsuke Kanda on 2016/08/29.
//

#ifndef CDA_TRIES_BASIC_HPP
#define CDA_TRIES_BASIC_HPP

#include <ctime>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace cda_tries {

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

constexpr uint32_t NOT_FOUND = UINT32_MAX;
constexpr uint32_t BC_UPPER  = UINT32_MAX >> 1;

enum class bc_type {
  PLAIN,
  DAC,
  FDAC
};

class bc_t {
public:
  bc_t() : base_(0), leaf_flag_(0), check_(0), fixed_flag_(0) {}
  ~bc_t() {}

  uint32_t base() const { return base_; }
  uint32_t link() const { return base_; }
  uint32_t check() const { return check_; }
  bool is_leaf() const { return leaf_flag_ == 1; }
  bool is_fixed() const { return fixed_flag_ == 1; }

  void set_base(uint32_t value) { base_ = value; leaf_flag_ = 0; }
  void set_link(uint32_t value) { base_ = value; leaf_flag_ = 1; }
  void set_check(uint32_t value) { check_ = value; }
  void fix() { fixed_flag_ = 1; }
  void unfix() { fixed_flag_ = 0; }

private:
  uint32_t base_       : 31;
  uint32_t leaf_flag_  : 1;
  uint32_t check_      : 31;
  uint32_t fixed_flag_ : 1;
};

enum class sw_type {
  SEC,
  MILLI,
  MICRO
};

class StopWatch {
public:
  StopWatch() : cl_(std::clock()) {}
  ~StopWatch() {}

  double get(sw_type type) const {
    double sec = 1.0 * (std::clock() - cl_) / CLOCKS_PER_SEC;
    switch (type) {
      case sw_type::SEC:
        break;
      case sw_type::MILLI:
        sec *= 1000.0;
        break;
      case sw_type::MICRO:
        sec *= 1000000.0;
        break;
    }
    return sec;
  }

  StopWatch(const StopWatch &) = delete;
  StopWatch &operator=(const StopWatch &) = delete;

private:
  std::clock_t cl_;
};

class suffix_t {
public:
  suffix_t() {}
  suffix_t(const char *str, size_t size, uint32_t node_pos)
    : str_(str + size - 1), size_(size), node_pos_(node_pos) {}
  ~suffix_t() {}

  char operator[](size_t pos) const { return *(str_ - pos); }
  const char *str() const { return str_ - size_ + 1; }
  size_t size() const { return size_; }
  uint32_t node_pos() const { return node_pos_; }

private:
  const char *str_   = nullptr;
  size_t size_       = 0;
  uint32_t node_pos_ = 0;
};

inline void LoadStrings(std::istream &is, std::vector<std::string> &strs) {
  std::string line;
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    strs.push_back(line);
  }
  strs.shrink_to_fit();
}

inline uint32_t PopCount(uint32_t bits) {
  bits = ((bits & 0xAAAAAAAA) >> 1) + (bits & 0x55555555);
  bits = ((bits & 0xCCCCCCCC) >> 2) + (bits & 0x33333333);
  bits = ((bits >> 4) + bits) & 0x0F0F0F0F;
  bits += bits >> 8;
  bits += bits >> 16;
  return bits & 0x3F;
}

}

#endif //CDA_TRIES_BASIC_HPP
