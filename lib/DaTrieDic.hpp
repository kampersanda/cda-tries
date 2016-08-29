#ifndef CDA_TRIES_DA_TRIE_DIC_HPP
#define CDA_TRIES_DA_TRIE_DIC_HPP

#include "Bc.hpp"
#include "BitArray.hpp"
#include "CodeTable.hpp"

namespace cda_tries {

class DaTrieDic {
public:
  DaTrieDic();
  ~DaTrieDic();

  // Builds a dictinoary from sorted strs in lexicographical order.
  void build(const std::vector<std::string> &strs, bc_type type);

  // Retruns the ID if str is in the dictionary, otherwise NOT_FOUND.
  uint32_t lookup(const char *str) const;
  // Returns the string with str_id in [0,num_strs).
  void access(uint32_t str_id, std::string &ret) const;
  // Returns all stored strings in lexicographical order.
  void enumerate(std::vector<uint32_t> &ret) const;

  size_t num_strs() const;
  size_t max_length() const;

  size_t bc_size() const;
  size_t tail_size() const;
  size_t size_in_bytes() const;

  void status(std::ostream &os) const;

  void write(std::ostream &os) const;
  void read(std::istream &is, bc_type type);

  void clear();

  DaTrieDic(const DaTrieDic &) = delete;
  DaTrieDic &operator=(const DaTrieDic &) = delete;

private:
  std::unique_ptr<Bc> bc_;
  BitArray term_flags_;
  Array<char> tail_;
  CodeTable table_;

  size_t num_strs_   = 0;
  size_t max_length_ = 0;

  uint32_t to_str_id_(uint32_t node_pos) const {
    return term_flags_.rank(node_pos);
  };
  uint32_t to_node_pos_(uint32_t str_id) const {
    return term_flags_.select(str_id);
  };

  void enumerate_(uint32_t node_pos, std::vector<uint32_t> &ret) const;
};

} // cda_tries

#endif // CDA_TRIES_DA_TRIE_DIC_HPP
