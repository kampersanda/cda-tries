#include <algorithm>
#include <cassert>

#include "CdaTrieDic.hpp"

namespace cda_tries {
namespace previous_tries {

class CdaBuilder {
public:
  friend class CdaTrieDic;

  static constexpr uint32_t FREE_BLOCKS = 16;

  CdaBuilder(const CdaBuilder &) = delete;
  CdaBuilder &operator=(const CdaBuilder &) = delete;

private:
  struct extra_t {
    extra_t() : next(0), used_flag(0), prev(0), base_flag(0) {}
    uint32_t next      : 31;
    uint32_t used_flag : 1;
    uint32_t prev      : 31;
    uint32_t base_flag : 1;
  };

  const std::vector<std::string> &strs_;
  const CodeTable &table_;

  std::vector<cda_bc_t> bc_;
  std::vector<bool> term_flags_;
  std::vector<char> tail_;

  std::vector<extra_t> extras_;
  std::vector<uint8_t> edges_;
  std::vector<suffix_t> suffixes_;

  uint32_t emp_head_ = NOT_FOUND;
  size_t num_emps_ = 0;

  CdaBuilder(const std::vector<std::string> &strs, const CodeTable &table);
  ~CdaBuilder();

  size_t build_();
  void expand_();
  void fix_(uint32_t node_pos);
  void fix_block_(uint32_t block_pos);
  void arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos);
  uint32_t xcheck_() const;
  bool is_target_(uint32_t base) const;
  void unify_tail_();
  void append_tail_(size_t begin, size_t end, const char *str);
};

CdaBuilder::CdaBuilder(const std::vector<std::string> &strs, const CodeTable &table)
  : strs_(strs), table_(table) {}

CdaBuilder::~CdaBuilder() {}

size_t CdaBuilder::build_() {
  assert(!strs_.empty());
  assert(strs_.size() <= BC_UPPER);

  if (!bc_.empty()) {
    return 0;
  }

  size_t init_capa = 1;
  while (init_capa < strs_.size()) {
    init_capa <<= 1;
  }

  bc_.reserve(init_capa);
  term_flags_.reserve(init_capa);
  extras_.reserve(init_capa);
  edges_.reserve(256);
  suffixes_.reserve(strs_.size());

  expand_();
  fix_(0);
  arrange_(0, strs_.size(), 0, 0);
  unify_tail_();

  if (emp_head_ != NOT_FOUND) {
    auto pos = emp_head_;
    do {
      pos = extras_[pos].next;
      ++num_emps_;
    } while (emp_head_ != pos);
  }

  return num_emps_;
}

void CdaBuilder::expand_() {
  auto old_size = static_cast<uint32_t>(bc_.size());
  auto new_size = old_size + 256;

  for (uint32_t i = 0; i < 256; ++i) {
    bc_.push_back(cda_bc_t{});
    term_flags_.push_back(false);
    extras_.push_back(extra_t{});
  }

  for (auto i = old_size; i < new_size; ++i) {
    extras_[i].prev = i - 1;
    extras_[i].next = i + 1;
  }

  if (emp_head_ == NOT_FOUND) {
    extras_[old_size].prev = new_size - 1;
    extras_[new_size - 1].next = old_size;
    emp_head_ = old_size;
  } else {
    auto emp_tail = extras_[emp_head_].prev;
    extras_[old_size].prev = emp_tail;
    extras_[emp_tail].next = old_size;
    extras_[emp_head_].prev = new_size - 1;
    extras_[new_size - 1].next = emp_head_;
  }

  auto old_block_size = old_size / 256;
  if (FREE_BLOCKS <= old_block_size) {
    fix_block_(old_block_size - FREE_BLOCKS);
  }
}

void CdaBuilder::fix_(uint32_t node_pos) {
  assert(extras_[node_pos].used_flag == 0);
  extras_[node_pos].used_flag = 1;

  auto prev = extras_[node_pos].prev;
  auto next = extras_[node_pos].next;
  extras_[next].prev = prev;
  extras_[prev].next = next;

  if (node_pos == emp_head_) {
    emp_head_ = node_pos == next ? NOT_FOUND : next;
  }
}

void CdaBuilder::fix_block_(uint32_t block_pos) {
  auto begin = block_pos * 256;
  auto end = begin + 256;
  for (auto i = begin; i < end; ++i) {
    if (extras_[i].used_flag == 0) {
      fix_(i);
      ++num_emps_;
    }
  }
}

void CdaBuilder::arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos) {
  if (strs_[begin].size() == depth) {
    ++begin;
    term_flags_[node_pos] = true;
    if (begin == end) {
      bc_[node_pos].set_link(0);
      return;
    }
  }

  if (begin + 1 == end && !term_flags_[node_pos]) {
    assert(strs_[begin].size() != depth);
    term_flags_[node_pos] = true;
    auto &str = strs_[begin];
    suffix_t suffix(str.c_str() + depth, str.size() - depth, node_pos);
    suffixes_.push_back(suffix);
    return;
  }

  {
    edges_.clear();
    auto label = static_cast<uint8_t>(strs_[begin][depth]);
    for (auto i = begin + 1; i < end; ++i) {
      auto _label = static_cast<uint8_t>(strs_[i][depth]);
      if (label != _label) {
        edges_.push_back(label);
        assert(label <= _label);
        label = _label;
      }
    }
    edges_.push_back(label);
  }

  auto base = xcheck_();
  if (bc_.size() <= base) {
    expand_();
  }

  bc_[node_pos].set_base(base);
  extras_[base].base_flag = 1;

  for (auto label : edges_) {
    auto child_pos = base ^ table_.code(label);
    fix_(child_pos);
    bc_[child_pos].set_check(label);
  }

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

uint32_t CdaBuilder::xcheck_() const {
  if (emp_head_ == NOT_FOUND) {
    return static_cast<uint32_t>(bc_.size()) ^ edges_[0];
  }

  auto pos = emp_head_;
  do {
    auto base = pos ^ table_.code(edges_[0]);
    if (is_target_(base)) {
      return base;
    }
    pos = extras_[pos].next;
  } while (emp_head_ != pos);

  return static_cast<uint32_t>(bc_.size()) ^ edges_[0];
}

bool CdaBuilder::is_target_(uint32_t base) const {
  if (extras_[base].base_flag == 1) {
    return false;
  }

  for (auto label : edges_) {
    auto pos = base ^ table_.code(label);
    if (extras_[pos].used_flag == 1) {
      return false;
    }
  }

  return true;
}

void CdaBuilder::unify_tail_() {
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

void CdaBuilder::append_tail_(size_t begin, size_t end, const char *str) {
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

CdaTrieDic::CdaTrieDic() {}

CdaTrieDic::~CdaTrieDic() {}

void CdaTrieDic::build(const std::vector<std::string> &strs) {
  clear();
  if (strs.empty()) {
    return;
  }

  num_strs_ = strs.size();
  table_.build(strs);

  CdaBuilder builder(strs, table_);
  num_emps_ = builder.build_();

  bc_.build(builder.bc_);
  term_flags_.build(builder.term_flags_);
  tail_.build(builder.tail_);
}

uint32_t CdaTrieDic::lookup(const char *str) const {
  uint32_t node_pos = 0;

  while (!bc_[node_pos].is_leaf()) {
    if (*str == '\0') {
      return term_flags_[node_pos] ? to_str_id_(node_pos) : NOT_FOUND;
    }
    node_pos = find_child_(node_pos, static_cast<uint8_t>(*str++));
    if (node_pos == NOT_FOUND) {
      return NOT_FOUND;
    }
  }

  auto tail = &tail_[bc_[node_pos].link()];
  while (*tail != '\0' && *tail == *str) {
    ++tail;
    ++str;
  }
  return (*tail == *str) ? to_str_id_(node_pos) : NOT_FOUND;
}

void CdaTrieDic::enumerate(std::vector<std::string> &ret) const {
  ret.clear();
  ret.reserve(num_strs_);
  enumerate_(0, std::string(), ret);
}

size_t CdaTrieDic::num_strs() const {
  return num_strs_;
}

size_t CdaTrieDic::bc_size() const {
  return bc_.size();
}

size_t CdaTrieDic::num_emps() const {
  return num_emps_;
}

size_t CdaTrieDic::tail_size() const {
  return tail_.size();
}

size_t CdaTrieDic::size_in_bytes() const {
  size_t size = 0;
  size += bc_.size_in_bytes();
  size += term_flags_.size_in_bytes();
  size += tail_.size_in_bytes();
  size += table_.size_in_bytes();
  return size;
}

void CdaTrieDic::status(std::ostream &os) const {
  os << "CDA trie dictionary status..." << std::endl;
  os << "num strs     : " << num_strs() << std::endl;
  os << "bc size      : " << bc_size() << std::endl;
  os << "num emps     : " << num_emps() << std::endl;
  os << "tail size    : " << tail_size() << std::endl;
  os << "size in bytes: " << size_in_bytes() << std::endl;
}

void CdaTrieDic::write(std::ostream &os) const {
  bc_.write(os);
  term_flags_.write(os);
  tail_.write(os);
  table_.write(os);
  os.write(reinterpret_cast<const char *>(&num_strs_), sizeof(num_strs_));
  os.write(reinterpret_cast<const char *>(&num_emps_), sizeof(num_emps_));
}

void CdaTrieDic::read(std::istream &is) {
  clear();
  bc_.read(is);
  term_flags_.read(is);
  tail_.read(is);
  table_.read(is);
  is.read(reinterpret_cast<char *>(&num_strs_), sizeof(num_strs_));
  is.read(reinterpret_cast<char *>(&num_emps_), sizeof(num_emps_));
}

void CdaTrieDic::clear() {
  bc_.clear();
  term_flags_.clear();
  tail_.clear();
  table_.clear();
  num_strs_   = 0;
  num_emps_   = 0;
}

void CdaTrieDic::enumerate_(uint32_t node_pos, const std::string &prefix,
                            std::vector<std::string> &ret) const {
  if (term_flags_[node_pos]) {
    if (bc_[node_pos].is_leaf()) {
      auto tail = &tail_[bc_[node_pos].link()];
      ret.push_back(prefix + tail);
      return;
    } else {
      ret.push_back(prefix);
    }
  }

  for (uint8_t label = 1;; ++label) {
    auto child_pos = find_child_(node_pos, label);
    if (child_pos != NOT_FOUND) {
      enumerate_(child_pos, prefix + static_cast<char>(label), ret);
    }
    if (label == UINT8_MAX) {
      break;
    }
  }
}

} // previous_tries
} // cda_tries
