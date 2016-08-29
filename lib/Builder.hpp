#ifndef CDA_TRIES_BUILDER_HPP
#define CDA_TRIES_BUILDER_HPP

#include "Bc.hpp"
#include "CodeTable.hpp"

namespace cda_tries {

class Builder {
public:
  friend class DaTrieDic;

  static constexpr uint32_t FREE_BLOCKS = 16;

  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;

private:
  const std::vector<std::string> &strs_;
  const CodeTable &table_;

  std::vector<bc_t> bc_;
  std::vector<bool> term_flags_;
  std::vector<char> tail_;

  std::vector<uint8_t> edges_;
  std::vector<suffix_t> suffixes_;
  std::vector<uint32_t> emp_heads_;

  uint32_t emp_head_   = NOT_FOUND;
  uint32_t block_size_ = 0;

  Builder(const std::vector<std::string> &strs, const CodeTable &table);
  ~Builder();

  void build_(bc_type type);
  void expand_();
  void fix_(uint32_t pos);
  void fix_block_(uint32_t block_pos);
  void arrange_(size_t begin, size_t end, size_t depth, uint32_t node_pos);
  uint32_t xcheck_() const;
  uint32_t ycheck_(uint32_t block_pos) const;
  bool is_target_(uint32_t base) const;
  void unify_tail_();
  void append_tail_(size_t begin, size_t end, const char *str);

  uint32_t prev_(uint32_t pos) const {
    return bc_[pos].base();
  }
  uint32_t next_(uint32_t pos) const {
    return bc_[pos].check();
  }
  void set_prev_(uint32_t pos, uint32_t prev) {
    bc_[pos].set_base(prev);
  }
  void set_next_(uint32_t pos, uint32_t next) {
    bc_[pos].set_check(next);
  }
};

} // cda_tries

#endif // CDA_TRIES_BUILDER_HPP
