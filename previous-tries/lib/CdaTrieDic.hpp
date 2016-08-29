#ifndef PREVIOUS_CDA_TRIE_DIC_HPP
#define PREVIOUS_CDA_TRIE_DIC_HPP

#include <cstring>

#include <BitArray.hpp>
#include <CodeTable.hpp>

#include "PrevDaTrieDic.hpp"

namespace cda_tries {
namespace previous_tries {

class cda_bc_t {
public:
  cda_bc_t() {}
  ~cda_bc_t() {}

  uint32_t base() const {
    uint32_t ret;
    std::memcpy(&ret, &elem_[0], sizeof(ret));
    return ret >> 1;
  }
  uint32_t link() const { return base(); }
  uint8_t check() const { return elem_[4]; }
  bool is_leaf() const { return (elem_[0] & 1) == 1; }

  void set_base(uint32_t base) {
    base <<= 1;
    std::memcpy(&elem_[0], &base, sizeof(base));
  }
  void set_link(uint32_t link) {
    link = (link << 1) | 1U;
    std::memcpy(&elem_[0], &link, sizeof(link));
  }
  void set_check(uint8_t check) { elem_[4] = check; }

private:
  uint8_t elem_[5] = {};
};

class CdaTrieDic : public PrevDaTrieDic {
public:
  CdaTrieDic();
  ~CdaTrieDic();

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

  CdaTrieDic(const CdaTrieDic &) = delete;
  CdaTrieDic &operator=(const CdaTrieDic &) = delete;

private:
  Array<cda_bc_t> bc_;
  BitArray term_flags_;
  Array<char> tail_;
  CodeTable table_;

  size_t num_strs_ = 0;
  size_t num_emps_ = 0;

  uint32_t to_str_id_(uint32_t node_pos) const {
    return term_flags_.rank(node_pos);
  };

  uint32_t find_child_(uint32_t node_pos, uint8_t label) const {
    node_pos = bc_[node_pos].base() ^ table_.code(label);
    return bc_[node_pos].check() == label ? node_pos : NOT_FOUND;
  }
  void enumerate_(uint32_t node_pos, const std::string &prefix, std::vector<std::string> &ret) const;
};

} // previous_tries
} // cda_tries

#endif //PREVIOUS_CDA_TRIE_DIC_HPP
