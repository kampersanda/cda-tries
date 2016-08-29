#include <algorithm>

#include "Builder.hpp"
#include "DaTrieDic.hpp"

namespace cda_tries {

DaTrieDic::DaTrieDic() {}

DaTrieDic::~DaTrieDic() {}

void DaTrieDic::build(const std::vector<std::string> &strs, bc_type type) {
  clear();
  if (strs.empty()) {
    return;
  }

  num_strs_ = strs.size();
  max_length_ = table_.build(strs);

  Builder builder(strs, table_);
  builder.build_(type);

  bc_ = Bc::create(type);
  bc_->build(builder.bc_);
  term_flags_.build(builder.term_flags_);
  tail_.build(builder.tail_);
}

uint32_t DaTrieDic::lookup(const char *str) const {
  uint32_t node_pos = 0;

  while (!bc_->is_leaf(node_pos)) {
    if (*str == '\0') {
      return term_flags_[node_pos] ? to_str_id_(node_pos) : NOT_FOUND;
    }
    auto label = static_cast<uint8_t>(*str++);
    auto child_pos = bc_->base(node_pos) ^ table_.code(label);
    if (child_pos == 0 || bc_->check(child_pos) != node_pos) {
      return NOT_FOUND;
    }
    node_pos = child_pos;
  }

  auto tail = &tail_[bc_->link(node_pos)];
  while (*tail != '\0' && *tail == *str) {
    ++tail;
    ++str;
  }
  return (*tail == *str) ? to_str_id_(node_pos) : NOT_FOUND;
}

void DaTrieDic::access(uint32_t str_id, std::string &ret) const {
  ret.clear();
  if (num_strs_ <= str_id) {
    return;
  }
  ret.reserve(max_length_);

  auto node_pos = to_node_pos_(str_id);
  auto tail_pos = bc_->is_leaf(node_pos) ? bc_->link(node_pos) : 0;

  while (node_pos != 0) {
    auto parent_pos = bc_->check(node_pos);
    auto code = static_cast<uint8_t>(bc_->base(parent_pos) ^ node_pos);
    ret += table_.label(code);
    node_pos = parent_pos;
  }

  std::reverse(ret.begin(), ret.end());
  ret += &tail_[tail_pos];
}

void DaTrieDic::enumerate(std::vector<uint32_t> &ret) const {
  ret.clear();
  ret.reserve(num_strs_);
  enumerate_(0, ret);
}

size_t DaTrieDic::num_strs() const {
  return num_strs_;
}

size_t DaTrieDic::max_length() const {
  return max_length_;
}

size_t DaTrieDic::bc_size() const {
  return bc_ ? bc_->size() : 0;
}

size_t DaTrieDic::tail_size() const {
  return tail_.size();
}

size_t DaTrieDic::size_in_bytes() const {
  size_t size = 0;
  size += bc_ ? bc_->size_in_bytes() : 0;
  size += term_flags_.size_in_bytes();
  size += tail_.size_in_bytes();
  size += table_.size_in_bytes();
  return size;
}

void DaTrieDic::status(std::ostream &os) const {
  os << "Double-array trie dictionary status..." << std::endl;
  os << "num strs     : " << num_strs() << std::endl;
  os << "bc size      : " << bc_size() << std::endl;
  os << "tail size    : " << tail_size() << std::endl;
  os << "size in bytes: " << size_in_bytes() << std::endl;
  bc_->status(os);
}

void DaTrieDic::write(std::ostream &os) const {
  bc_->write(os);
  term_flags_.write(os);
  tail_.write(os);
  table_.write(os);
  os.write(reinterpret_cast<const char *>(&num_strs_), sizeof(num_strs_));
  os.write(reinterpret_cast<const char *>(&max_length_), sizeof(max_length_));
}

void DaTrieDic::read(std::istream &is, bc_type type) {
  clear();
  bc_ = Bc::create(type);
  bc_->read(is);
  term_flags_.read(is);
  tail_.read(is);
  table_.read(is);
  is.read(reinterpret_cast<char *>(&num_strs_), sizeof(num_strs_));
  is.read(reinterpret_cast<char *>(&max_length_), sizeof(max_length_));
}

void DaTrieDic::clear() {
  bc_.reset();
  term_flags_.clear();
  tail_.clear();
  table_.clear();
  num_strs_   = 0;
  max_length_ = 0;
}

void DaTrieDic::enumerate_(uint32_t node_pos, std::vector<uint32_t> &ret) const {
  if (term_flags_[node_pos]) {
    ret.push_back(to_str_id_(node_pos));
  }
  if (bc_->is_leaf(node_pos)) {
    return;
  }

  auto base = bc_->base(node_pos);
  for (uint8_t label = 0;; ++label) {
    auto child_pos = base ^ table_.code(label);
    if (child_pos != 0 && bc_->check(child_pos) == node_pos) {
      enumerate_(child_pos, ret);
    }
    if (label == UINT8_MAX) {
      break;
    }
  }
}

} // cda_tries
