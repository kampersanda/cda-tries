#ifndef PREVIOUS_DALF_TRIE_DIC_HPP
#define PREVIOUS_DALF_TRIE_DIC_HPP

#include <BitArray.hpp>
#include <CodeTable.hpp>
#include <SmallArray.hpp>

#include "PrevDaTrieDic.hpp"

namespace cda_tries {
namespace previous_tries {

constexpr uint32_t FIXED_SHIFT = 23;
inline constexpr uint32_t float2fixed(float value) {
  return static_cast<uint32_t>(value * (1U << FIXED_SHIFT));
}

constexpr uint8_t  INVALID_BASE = UINT8_MAX;
constexpr uint32_t BLOCK_SIZE   = 512;
constexpr uint32_t ALPHA        = 128;
constexpr uint32_t GAIN         = float2fixed(1.0);
constexpr float    EX_RATIO     = 2.0; // it's heuristic

class dalf_bc_t {
public:
  dalf_bc_t() {}
  ~dalf_bc_t() {}

  uint8_t base() const { return base_; }
  uint8_t check() const { return check_; }
  void set_base(uint8_t base) { base_ = base; }
  void set_check(uint8_t check) { check_ = check; }

private:
  uint8_t base_  = INVALID_BASE;
  uint8_t check_ = '\0';
};

class linear_func_t {
public:
  linear_func_t() {}
  ~linear_func_t() {}

  uint32_t get(uint32_t offset) const {
    auto ans = uint32_t(uint64_t(slope_) * offset >> FIXED_SHIFT) + head_;
    return ans < ALPHA ? 0 : ans - ALPHA;
  }
  uint32_t head() const { return head_; }

  void set(float slope, uint32_t head) {
    slope_ = float2fixed(slope);
    head_ = head;
  }
  void update() { slope_ += GAIN; }

private:
  uint32_t slope_ = 0;
  uint32_t head_  = 0;
};

class DalfTrieDic : public PrevDaTrieDic {
public:
  DalfTrieDic();
  ~DalfTrieDic();

  void build(const std::vector<std::string> &strs);

  uint32_t lookup(const char *str) const;
  void enumerate(std::vector<std::string> &ret) const;

  size_t num_strs() const;
  size_t bc_size() const;
  size_t num_emps() const;
  size_t tail_size() const;
  size_t size_in_bytes() const;
  void status(std::ostream &os) const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  DalfTrieDic(const DalfTrieDic &) = delete;
  DalfTrieDic &operator=(const DalfTrieDic &) = delete;

private:
  Array<dalf_bc_t> bc_;
  Array<linear_func_t> linear_funcs_;
  BitArray term_flags_;
  BitArray leaf_flags_;
  SmallArray links_;
  Array<char> tail_;
  CodeTable table_;

  size_t num_strs_ = 0;
  size_t num_emps_ = 0;
  size_t num_rearrange_ = 0;

  uint32_t to_str_id_(uint32_t node_pos) const {
    return term_flags_.rank(node_pos);
  };
  uint32_t link_(uint32_t node_pos) const {
    return bc_[node_pos].base() | (links_[leaf_flags_.rank(node_pos)] << 8);
  }

  uint32_t find_child_(uint32_t node_pos, uint8_t label) const {
    auto block_pos = node_pos / BLOCK_SIZE;
    auto offset = node_pos % BLOCK_SIZE;
    node_pos = bc_[node_pos].base() + linear_funcs_[block_pos].get(offset) + table_.code(label);
    if (node_pos < bc_.size() && bc_[node_pos].check() == label) {
      return node_pos;
    }
    return NOT_FOUND;
  }
  void enumerate_(uint32_t node_pos, const std::string &prefix, std::vector<std::string> &ret) const;
};

} // previous_tries
} // cda_tries

#endif //PREVIOUS_DALF_TRIE_DIC_HPP
