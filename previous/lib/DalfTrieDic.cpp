#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>

#include "DalfTrieDic.hpp"

namespace cda_tries {
namespace previous {

struct edge_t {
  uint8_t  label;
  uint32_t node_pos;
};

class ListTrie {
public:
  friend class DalfTrieDic;

  ListTrie() {}
  ~ListTrie() {}

  void build(const std::vector<std::string> &strs);

  static uint32_t root() { return 0; }
  bool is_term(uint32_t node_pos) const { return nodes_[node_pos].is_term == 1; }
  bool is_leaf(uint32_t node_pos) const { return nodes_[node_pos].degree == 0; }
  uint32_t link(uint32_t node_pos) const { return nodes_[node_pos].child; }

  size_t num_nodes() const { return nodes_.size(); }
  uint32_t degree(uint32_t node_pos) const { return nodes_[node_pos].degree; }
  float ave_degree() const { return static_cast<float>(total_degree_) / nodes_.size(); }
  void edges(uint32_t node_pos, std::vector<edge_t> &edges) const;

  ListTrie(const ListTrie &) = delete;
  ListTrie &operator=(const ListTrie &) = delete;

private:
  struct node_t {
    node_t(uint8_t _label = '\0') : label(_label), degree(0), is_term(0), child(root()) {}
    uint32_t label   : 8;
    uint32_t degree  : 9;
    uint32_t is_term : 1;
    uint32_t child;
  };
  std::vector<node_t> nodes_;
  std::vector<char> tail_;
  uint32_t total_degree_ = 0;

  void arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos,
                const std::vector<std::string> &strs, std::vector<suffix_t> &suffixes);
  void unify_tail_(std::vector<suffix_t> &suffixes);
  void append_tail_(size_t begin, size_t end, const char *str, std::vector<suffix_t> &suffixes);
};

void ListTrie::build(const std::vector<std::string> &strs) {
  std::vector<suffix_t> suffixes;

  size_t init_capa = 1;
  while (init_capa < strs.size()) {
    init_capa <<= 1;
  }

  nodes_.reserve(init_capa);
  suffixes.reserve(strs.size());

  nodes_.push_back(node_t{});
  arrange_(0, strs.size(), 0 , root(), strs, suffixes);
  unify_tail_(suffixes);
}

void ListTrie::edges(uint32_t node_pos, std::vector<edge_t> &edges) const {
  edges.clear();
  if (is_leaf(node_pos)) {
    return;
  }

  auto child_pos = nodes_[node_pos].child;
  for (uint32_t i = 0; i < nodes_[node_pos].degree; ++i) {
    edge_t edge;
    edge.label = static_cast<uint8_t>(nodes_[child_pos + i].label);
    edge.node_pos = child_pos + i;
    edges.push_back(edge);
  }
}

void ListTrie::arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos,
                        const std::vector<std::string> &strs, std::vector<suffix_t> &suffixes) {
  if (strs[begin].size() == depth) {
    ++begin;
    nodes_[node_pos].is_term = 1;
    if (begin == end) {
      nodes_[node_pos].child = 0;
      return;
    }
  }

  if (begin + 1 == end && !nodes_[node_pos].is_term) {
    assert(strs[begin].size() != depth);
    nodes_[node_pos].is_term = 1;
    auto &str = strs[begin];
    suffix_t suffix(str.c_str() + depth, str.size() - depth, node_pos);
    suffixes.push_back(suffix);
    return;
  }

  nodes_[node_pos].child = static_cast<uint32_t>(nodes_.size());
  {
    auto label = static_cast<uint8_t>(strs[begin][depth]);
    for (auto i = begin + 1; i < end; ++i) {
      auto _label = static_cast<uint8_t>(strs[i][depth]);
      if (label != _label) {
        nodes_.push_back(node_t{label});
        ++nodes_[node_pos].degree;

        assert(label <= _label);
        label = _label;
      }
    }
    nodes_.push_back(node_t{label});
    ++nodes_[node_pos].degree;
  }
  total_degree_ += nodes_[node_pos].degree;

  auto _begin = begin;
  auto label = static_cast<uint8_t>(strs[begin][depth]);
  auto child_pos = nodes_[node_pos].child;

  for (auto _end = begin + 1; _end < end; ++_end) {
    auto _label = static_cast<uint8_t>(strs[_end][depth]);
    if (label != _label) {
      arrange_(_begin, _end, depth + 1, child_pos++, strs, suffixes);
      label = _label;
      _begin = _end;
    }
  }
  arrange_(_begin, end, depth + 1, child_pos, strs, suffixes);
}

void ListTrie::unify_tail_(std::vector<suffix_t> &suffixes) {
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

  std::sort(suffixes.begin(), suffixes.end(), comp_suffix);
  tail_.push_back('\0');

  size_t begin = 0;
  for (size_t i = 1; i < suffixes.size(); ++i) {
    auto &lhs = suffixes[i - 1];
    auto &rhs = suffixes[i];

    if (can_unify(lhs, rhs)) {
      continue;
    }

    append_tail_(begin, i, lhs.str(), suffixes);
    begin = i;
  }

  append_tail_(begin, suffixes.size(), suffixes.back().str(), suffixes);
}

void ListTrie::append_tail_(size_t begin, size_t end, const char *str, std::vector<suffix_t> &suffixes) {
  while (*str != '\0') {
    tail_.push_back(*str++);
  }
  while (begin < end) {
    auto &suffix = suffixes[begin++];
    auto tail_pos = static_cast<uint32_t>(tail_.size() - suffix.size());
    assert(is_leaf(suffix.node_pos()));
    nodes_[suffix.node_pos()].child = tail_pos;
  }
  tail_.push_back('\0');
}

class DalfBuilder {
public:
  friend class DalfTrieDic;

  static constexpr uint32_t NOT_FOUND = UINT32_MAX;

  DalfBuilder(const DalfBuilder &) = delete;
  DalfBuilder &operator=(const DalfBuilder &) = delete;

private:
  struct extra_t {
    extra_t() : next(0), used_flag(0), prev(0), base_flag(0) {}
    uint32_t next      : 31;
    uint32_t used_flag : 1;
    uint32_t prev      : 31;
    uint32_t base_flag : 1;
  };

  struct node_t {
    uint32_t trie_pos;
    uint32_t dalf_pos;
    bool operator<(const node_t &rhs) const {
      return dalf_pos < rhs.dalf_pos;
    }
    bool operator>(const node_t &rhs) const {
      return dalf_pos > rhs.dalf_pos;
    }
  };

  const ListTrie &trie_;
  const CodeTable &table_;

  std::vector<dalf_bc_t> bc_;
  std::vector<linear_func_t> linear_funcs_;
  std::vector<bool> term_flags_;
  std::vector<bool> leaf_flags_;
  std::vector<uint32_t> links_;

  std::vector<extra_t> extras_;
  std::vector<edge_t> edges_;
  std::vector<node_t> nodes_;
  std::vector<node_t> leaves_;
  std::vector<node_t> children_;

  uint32_t emp_head_   = 0;
  uint32_t length_ = 0;
  size_t num_rearrange_ = 0;

  using q_t = std::priority_queue<node_t, std::vector<node_t>, std::greater<node_t>>;
  q_t q_;

  DalfBuilder(const ListTrie &trie, const CodeTable &table);
  ~DalfBuilder();

  size_t build_();
  void reserve_();
  void arrange_();
  bool arrange_in_1st_block_();
  bool arrange_in_block_();
  uint32_t arrange_in_node_(node_t node);
  void arrange_leaves_();
  uint32_t xcheck_(uint32_t base_lower) const;
  bool is_target_(uint32_t base) const;
  void fix_(uint32_t node_pos);
  void reset_(uint32_t block_pos);
};

DalfBuilder::DalfBuilder(const ListTrie &trie, const CodeTable &table)
  : trie_(trie), table_(table) {}

DalfBuilder::~DalfBuilder() {}

size_t DalfBuilder::build_() {
  reserve_();
  arrange_();

  bc_.resize(length_);
  linear_funcs_.resize(length_ / BLOCK_SIZE + 1);
  term_flags_.resize(length_);
  leaf_flags_.resize(length_);

  return num_rearrange_;
}

void DalfBuilder::reserve_() {
  auto max_length = static_cast<uint32_t>(trie_.num_nodes() * EX_RATIO);

  bc_.resize(max_length);
  linear_funcs_.resize(max_length / BLOCK_SIZE + 1);
  term_flags_.resize(max_length, false);
  leaf_flags_.resize(max_length, false);
  extras_.resize(max_length);

  for (uint32_t i = 0; i < max_length; ++i) {
    extras_[i].prev = i - 1;
    extras_[i].next = i + 1;
  }
  extras_[0].prev = max_length - 1;
  extras_[max_length - 1].next = 0;
}

void DalfBuilder::arrange_() {
  linear_funcs_[0].set(trie_.ave_degree(), 0);

  while (!arrange_in_1st_block_()) {
    reset_(0);
    q_ = q_t{};
  }
  arrange_leaves_();

  while (!q_.empty()) {
    nodes_.clear();

    auto block_pos = q_.top().dalf_pos / BLOCK_SIZE;
    uint32_t total_degree = 0;

    while (!q_.empty() && (q_.top().dalf_pos / BLOCK_SIZE) == block_pos) {
      total_degree += trie_.degree(q_.top().trie_pos);
      nodes_.push_back(q_.top());
      q_.pop();
    }

    linear_funcs_[block_pos].set(
      static_cast<float>(total_degree) / BLOCK_SIZE,
      std::max(length_, (block_pos + 1) * BLOCK_SIZE)
    );

    if (bc_.size() <= linear_funcs_[block_pos].head()) {
      std::cerr << "Please expand EX_RATIO because extra space is insufficient." << std::endl;
      exit(1);
    }

    while (emp_head_ < linear_funcs_[block_pos].head()) {
      fix_(emp_head_);
    }

    while (!arrange_in_block_()) {
      reset_(block_pos);
    }
    arrange_leaves_();
  }
}

bool DalfBuilder::arrange_in_1st_block_() {
  fix_(0);
  q_.push(node_t{trie_.root(), 0});

  while (!q_.empty() && q_.top().dalf_pos / BLOCK_SIZE == 0) {
    node_t node = q_.top();
    q_.pop();

    if (trie_.is_leaf(node.trie_pos)) {
      leaves_.push_back(node);
      continue;
    }

    auto base = arrange_in_node_(node);
    if (base == NOT_FOUND) {
      return false;
    }

    for (const auto &edge : edges_) {
      node_t child;
      child.trie_pos = edge.node_pos;
      child.dalf_pos = base + table_.code(edge.label);
      q_.push(child);
    }
  }

  return true;
}

bool DalfBuilder::arrange_in_block_() {
  children_.clear();

  for (const auto &node : nodes_) {
    if (trie_.is_leaf(node.trie_pos)) {
      leaves_.push_back(node);
      continue;
    }

    auto base = arrange_in_node_(node);
    if (base == NOT_FOUND) {
      return false;
    }

    for (const auto &edge : edges_) {
      node_t child;
      child.trie_pos = edge.node_pos;
      child.dalf_pos = base + table_.code(edge.label);
      children_.push_back(child);
    }
  }

  for (const auto &child : children_) {
    q_.push(child);
  }

  return true;
}

uint32_t DalfBuilder::arrange_in_node_(node_t node) {
  assert(!trie_.is_leaf(node.trie_pos));
  term_flags_[node.dalf_pos] = trie_.is_term(node.trie_pos);

  edges_.clear();
  trie_.edges(node.trie_pos, edges_);

  auto base_lower = linear_funcs_[node.dalf_pos / BLOCK_SIZE].get(node.dalf_pos % BLOCK_SIZE);
  auto base = xcheck_(base_lower);
  if (base == NOT_FOUND) {
    std::cerr << "Please expand EX_RATIO because extra space is insufficient." << std::endl;
    exit(1);
  }
  assert(base_lower <= base);
  if (base_lower + INVALID_BASE <= base) {
    return NOT_FOUND;
  }

  bc_[node.dalf_pos].set_base(static_cast<uint8_t>(base - base_lower));
  extras_[base].base_flag = 1;

  for (const auto &edge : edges_) {
    auto child_pos = base + table_.code(edge.label);
    fix_(child_pos);
    bc_[child_pos].set_check(edge.label);
  }

  return base;
}

void DalfBuilder::arrange_leaves_() {
  std::sort(leaves_.begin(), leaves_.end());
  for (const auto &leaf : leaves_) {
    assert(trie_.is_leaf(leaf.trie_pos));
    uint32_t tail_pos = trie_.link(leaf.trie_pos);
    bc_[leaf.dalf_pos].set_base(static_cast<uint8_t>(tail_pos & 0xFF));
    links_.push_back(tail_pos >> 8);
    leaf_flags_[leaf.dalf_pos] = true;
    term_flags_[leaf.dalf_pos] = true;
  }
  leaves_.clear();
}

uint32_t DalfBuilder::xcheck_(uint32_t base_lower) const {
  auto cmp = [=](const edge_t &lhs, const edge_t &rhs) {
    return table_.code(lhs.label) < table_.code(rhs.label);
  };
  const auto &min_edge = *min_element(edges_.begin(), edges_.end(), cmp);

  auto pos = emp_head_;
  do {
    if (table_.code(min_edge.label) <= pos) {
      auto base = pos - table_.code(min_edge.label);
      if (base_lower <= base && is_target_(base)) {
        return base;
      }
    }
    pos = extras_[pos].next;
  } while (pos != emp_head_);

  return NOT_FOUND;
}

bool DalfBuilder::is_target_(uint32_t base) const {
  if (extras_[base].base_flag == 1) {
    return false;
  }
  for (const auto &edge : edges_) {
    auto child_pos = base + table_.code(edge.label);
    if (bc_.size() <= child_pos) {
      std::cerr << "Please expand EX_RATIO because extra space is insufficient." << std::endl;
      exit(1);
    }
    if (extras_[child_pos].used_flag == 1) {
      return false;
    }
  }
  return true;
}

void DalfBuilder::fix_(uint32_t node_pos) {
  assert(node_pos < bc_.size());
  assert(extras_[node_pos].used_flag == 0);

  length_ = std::max(length_, node_pos + 1);
  extras_[node_pos].used_flag = 1;

  auto prev = extras_[node_pos].prev;
  auto next = extras_[node_pos].next;
  extras_[next].prev = prev;
  extras_[prev].next = next;

  if (node_pos == emp_head_) {
    emp_head_ = next;
  }
}

void DalfBuilder::reset_(uint32_t block_pos) {
  { // reset base
    auto begin = block_pos * BLOCK_SIZE;
    auto end = begin + BLOCK_SIZE;

    for (auto i = begin; i < end; ++i) {
      assert(!leaf_flags_[i]);
      if (bc_[i].base() != INVALID_BASE) {
        auto base_lower = linear_funcs_[i / BLOCK_SIZE].get(i % BLOCK_SIZE);
        auto base = bc_[i].base() + base_lower;
        assert(extras_[base].base_flag == 1);
        extras_[base].base_flag = 0;
        bc_[i].set_base(INVALID_BASE);
      }
    }
  }

  { // reset elems
    auto last_pos = extras_[emp_head_].prev;
    emp_head_ = linear_funcs_[block_pos].head();
    for (auto i = emp_head_; i < length_; ++i) {
      extras_[i].prev = i - 1;
      extras_[i].next = i + 1;
      extras_[i].used_flag = 0;
      bc_[i].set_check('\0');
      term_flags_[i] = false;
    }
    extras_[last_pos].next = emp_head_;
    extras_[emp_head_].prev = last_pos;
    extras_[length_].prev = length_ - 1;
    length_ = emp_head_;
  }

  linear_funcs_[block_pos].update();
  leaves_.clear();

  ++num_rearrange_;
}

DalfTrieDic::DalfTrieDic() {}

DalfTrieDic::~DalfTrieDic() {}

void DalfTrieDic::build(const std::vector<std::string> &strs) {
  clear();
  if (strs.empty()) {
    return;
  }

  num_strs_ = strs.size();
  table_.build(strs);

  // via list trie
  ListTrie trie;
  trie.build(strs);

  DalfBuilder builder(trie, table_);
  num_rearrange_ = builder.build_();

  bc_.build(builder.bc_);
  linear_funcs_.build(builder.linear_funcs_);
  term_flags_.build(builder.term_flags_);
  leaf_flags_.build(builder.leaf_flags_);
  links_.build(builder.links_);
  tail_.build(trie.tail_);

  num_emps_ = bc_.size() - trie.num_nodes();
}

uint32_t DalfTrieDic::lookup(const char *str) const {
  uint32_t node_pos = 0;

  while (!leaf_flags_[node_pos]) {
    if (*str == '\0') {
      return term_flags_[node_pos] ? to_str_id_(node_pos) : NOT_FOUND;
    }
    node_pos = find_child_(node_pos, static_cast<uint8_t>(*str++));
    if (node_pos == NOT_FOUND) {
      return NOT_FOUND;
    }
  }

  auto tail = &tail_[link_(node_pos)];
  while (*tail != '\0' && *tail == *str) {
    ++tail;
    ++str;
  }
  return (*tail == *str) ? to_str_id_(node_pos) : NOT_FOUND;
}

void DalfTrieDic::enumerate(std::vector<std::string> &ret) const {
  ret.clear();
  ret.reserve(num_strs_);
  enumerate_(0, std::string(), ret);
}

size_t DalfTrieDic::num_strs() const {
  return num_strs_;
}

size_t DalfTrieDic::bc_size() const {
  return bc_.size();
}

size_t DalfTrieDic::num_emps() const {
  return num_emps_;
}

size_t DalfTrieDic::tail_size() const {
  return tail_.size();
}

size_t DalfTrieDic::size_in_bytes() const {
  size_t size = 0;
  size += bc_.size_in_bytes();
  size += linear_funcs_.size_in_bytes();
  size += term_flags_.size_in_bytes();
  size += leaf_flags_.size_in_bytes();
  size += links_.size_in_bytes();
  size += tail_.size_in_bytes();
  size += table_.size_in_bytes();
  return size;
}

void DalfTrieDic::status(std::ostream &os) const {
  os << "DALF trie dictionary status..." << std::endl;
  os << "num strs     : " << num_strs() << std::endl;
  os << "bc size      : " << bc_size() << std::endl;
  os << "num emps     : " << num_emps() << std::endl;
  os << "tail size    : " << tail_size() << std::endl;
  os << "size in bytes: " << size_in_bytes() << std::endl;
  os << "num_rearrange: " << num_rearrange_ << std::endl;
}

void DalfTrieDic::write(std::ostream &os) const {
  bc_.write(os);
  linear_funcs_.write(os);
  term_flags_.write(os);
  leaf_flags_.write(os);
  links_.write(os);
  tail_.write(os);
  table_.write(os);
  os.write(reinterpret_cast<const char *>(&num_strs_), sizeof(num_strs_));
  os.write(reinterpret_cast<const char *>(&num_emps_), sizeof(num_emps_));
  os.write(reinterpret_cast<const char *>(&num_rearrange_), sizeof(num_rearrange_));
}

void DalfTrieDic::read(std::istream &is) {
  clear();
  bc_.read(is);
  linear_funcs_.read(is);
  term_flags_.read(is);
  leaf_flags_.read(is);
  links_.read(is);
  tail_.read(is);
  table_.read(is);
  is.read(reinterpret_cast<char *>(&num_strs_), sizeof(num_strs_));
  is.read(reinterpret_cast<char *>(&num_emps_), sizeof(num_emps_));
  is.read(reinterpret_cast<char *>(&num_rearrange_), sizeof(num_rearrange_));
}

void DalfTrieDic::clear() {
  bc_.clear();
  linear_funcs_.clear();
  term_flags_.clear();
  leaf_flags_.clear();
  links_.clear();
  tail_.clear();
  table_.clear();
  num_strs_ = 0;
  num_emps_ = 0;
  num_rearrange_ = 0;
}

void DalfTrieDic::enumerate_(uint32_t node_pos, const std::string &prefix,
                             std::vector<std::string> &ret) const {
  if (term_flags_[node_pos]) {
    if (leaf_flags_[node_pos]) {
      auto tail = &tail_[link_(node_pos)];
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

} // previous
} // cda_tries
