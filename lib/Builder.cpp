#include <algorithm>
#include <cassert>
#include <iostream>

#include "Builder.hpp"

namespace cda_tries {

Builder::Builder(const std::vector<std::string> &strs, const CodeTable &table)
  : strs_(strs), table_(table) {}

Builder::~Builder() {}

void Builder::build_(bc_type type) {
  if (BC_UPPER < strs_.size()) {
    std::cerr << "Critical error: dic size is too large" << std::endl;
    exit(1);
  }

  if (!bc_.empty()) {
    return;
  }

  switch (type) {
    case bc_type::PLAIN:
      block_size_ = 0;
      break;
    case bc_type::DAC:
      block_size_ = 256;
      break;
    case bc_type::FDAC:
      block_size_ = 128;
      break;
  }
  emp_head_ = NOT_FOUND;

  size_t init_capa = 1;
  while (init_capa < strs_.size()) {
    init_capa <<= 1;
  }

  bc_.reserve(init_capa);
  term_flags_.reserve(init_capa);

  edges_.reserve(256);
  suffixes_.reserve(strs_.size());
  if (block_size_) {
    emp_heads_.reserve(init_capa / block_size_);
  }

  expand_();
  fix_(0);
  arrange_(0, strs_.size(), 0, 0);
  unify_tail_();
}

void Builder::expand_() {
  if (BC_UPPER < bc_.size() + 256) {
    std::cerr << "Critical error: dic size is too large" << std::endl;
    exit(1);
  }

  auto old_size = static_cast<uint32_t>(bc_.size());
  auto new_size = old_size + 256;

  for (uint32_t i = 0; i < 256; ++i) {
    bc_.push_back(bc_t{});
    term_flags_.push_back(false);
  }

  for (auto pos = old_size; pos < new_size; ++pos) {
    set_prev_(pos, pos - 1);
    set_next_(pos, pos + 1);
  }

  if (emp_head_ == NOT_FOUND) {
    set_prev_(old_size, new_size - 1);
    set_next_(new_size - 1, old_size);
    emp_head_ = old_size;
  } else {
    auto emp_tail = prev_(emp_head_);
    set_prev_(old_size, emp_tail);
    set_next_(emp_tail, old_size);
    set_prev_(emp_head_, new_size - 1);
    set_next_(new_size - 1, emp_head_);
  }

  if (block_size_) {
    for (auto pos = old_size; pos < new_size; pos += block_size_) {
      emp_heads_.push_back(pos);
    }
  }

  auto old_block_size = old_size / 256;
  if (FREE_BLOCKS <= old_block_size) {
    fix_block_(old_block_size - FREE_BLOCKS);
  }
}

void Builder::fix_(uint32_t pos) {
  assert(!bc_[pos].is_fixed());
  bc_[pos].fix();

  auto prev = prev_(pos);
  auto next = next_(pos);
  set_prev_(next, prev);
  set_next_(prev, next);

  if (pos == emp_head_) {
    emp_head_ = pos == next ? NOT_FOUND : next;
  }

  if (block_size_) {
    auto block_pos = pos / block_size_;
    if (pos != emp_heads_[block_pos]) {
      return;
    }
    if (emp_head_ == NOT_FOUND || block_pos != next / block_size_) {
      emp_heads_[block_pos] = NOT_FOUND;
    } else {
      emp_heads_[block_pos] = next;
    }
  }
}

void Builder::fix_block_(uint32_t block_pos) {
  auto begin = block_pos * 256;
  auto end = begin + 256;

  for (auto pos = begin; pos < end; ++pos) {
    if (!bc_[pos].is_fixed()) {
      fix_(pos);
      bc_[pos].unfix();
    }
  }

  if (block_size_) {
    for (auto pos = begin; pos < end; pos += block_size_) {
      emp_heads_[pos / block_size_] = NOT_FOUND;
    }
  }
}

void Builder::arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos) {
  if (strs_[begin].size() == depth) {
    ++begin;
    term_flags_[node_pos] = true;
    if (begin == end) { // without link
      bc_[node_pos].set_link(0);
      return;
    }
  }

  if (begin + 1 == end && !term_flags_[node_pos]) { // is leaf
    if (strs_[begin].size() == depth) {
      std::cerr << "Critical error: dic includes overlapped strs" << std::endl;
      exit(1);
    }
    term_flags_[node_pos] = true;
    auto &str = strs_[begin];
    suffix_t suffix(str.c_str() + depth, str.size() - depth, node_pos);
    suffixes_.push_back(suffix);
    return;
  }

  { // fetch edges
    edges_.clear();
    auto label = static_cast<uint8_t>(strs_[begin][depth]);
    for (auto str_id = begin + 1; str_id < end; ++str_id) {
      auto _label = static_cast<uint8_t>(strs_[str_id][depth]);
      if (label != _label) {
        if (_label < label) {
          std::cerr << "Critical error: dic is unsorted" << std::endl;
          exit(1);
        }
        edges_.push_back(label);
        label = _label;
      }
    }
    edges_.push_back(label);
  }

#ifdef ENABLE_YCHECK
  auto base = block_size_ ? ycheck_(node_pos / block_size_) : xcheck_();
#else
  auto base = xcheck_();
#endif

  if (bc_.size() <= base) {
    expand_();
  }

  // define
  bc_[node_pos].set_base(base);
  for (auto label : edges_) {
    auto child_pos = base ^ table_.code(label);
    fix_(child_pos);
    bc_[child_pos].set_check(node_pos);
  }

  // follow
  auto _begin = begin;
  auto label = static_cast<uint8_t>(strs_[begin][depth]);
  for (auto _end = begin + 1; _end < end; ++_end) {
    auto _label = static_cast<uint8_t>(strs_[_end][depth]);
    if (label != _label) {
      arrange_(_begin, _end, depth + 1, base ^ table_.code(label));
      label = _label;
      _begin = _end;
    }
  }
  arrange_(_begin, end, depth + 1, base ^ table_.code(label));
}

uint32_t Builder::xcheck_() const {
  if (emp_head_ == NOT_FOUND) {
    return static_cast<uint32_t>(bc_.size()) ^ edges_[0];
  }

  auto pos = emp_head_;
  do {
    auto base = pos ^ table_.code(edges_[0]);
    if (is_target_(base)) {
      return base;
    }
    pos = next_(pos);
  } while (emp_head_ != pos);

  return static_cast<uint32_t>(bc_.size()) ^ edges_[0];
}

uint32_t Builder::ycheck_(uint32_t block_pos) const {
  if (emp_head_ == NOT_FOUND) {
    return static_cast<uint32_t>(bc_.size()) ^ edges_[0];
  }

  auto pos = emp_heads_[block_pos];
  if (pos != NOT_FOUND) {
    do {
      auto base = pos ^ table_.code(edges_[0]);
      if (is_target_(base)) {
        return base;
      }
      pos = next_(pos);
      if (pos / block_size_ != block_pos) {
        break;
      }
    } while (emp_head_ != pos);
  }

  return xcheck_();
}

bool Builder::is_target_(uint32_t base) const {
  for (auto label : edges_) {
    auto pos = base ^ table_.code(label);
    if (bc_[pos].is_fixed()) {
      return false;
    }
  }
  return true;
}

void Builder::unify_tail_() {
  auto comp_suffix = [](const suffix_t &lhs, const suffix_t &rhs) {
    for (size_t i = 0; i < lhs.size(); ++i) {
      if (i == rhs.size()) {
        return false;
      }
      if (lhs[i] != rhs[i]) {
        return static_cast<uint8_t>(lhs[i]) < static_cast<uint8_t>(rhs[i]);
      }
    }
    return lhs.size() < rhs.size();
  };

  auto can_unify = [](const suffix_t &lhs, const suffix_t &rhs) {
    if (lhs.size() > rhs.size()) {
      return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
      if (lhs[i] != rhs[i]) {
        return false;
      }
    }
    return true;
  };

  std::sort(suffixes_.begin(), suffixes_.end(), comp_suffix);
  tail_.push_back('\0');

  size_t begin = 0;
  for (size_t i = 1; i < suffixes_.size(); ++i) {
    auto &lhs = suffixes_[i - 1];
    auto &rhs = suffixes_[i];

    if (can_unify(lhs, rhs)) {
      continue;
    }

    append_tail_(begin, i, lhs.str());
    begin = i;
  }

  append_tail_(begin, suffixes_.size(), suffixes_.back().str());
}

void Builder::append_tail_(size_t begin, size_t end, const char *str) {
  while (*str != '\0') {
    tail_.push_back(*str++);
  }
  while (begin < end) {
    auto &suffix = suffixes_[begin++];
    auto tail_pos = static_cast<uint32_t>(tail_.size() - suffix.size());
    assert(tail_pos <= BC_UPPER);
    bc_[suffix.node_pos()].set_link(tail_pos);
  }
  tail_.push_back('\0');
}

} // cda_tries
