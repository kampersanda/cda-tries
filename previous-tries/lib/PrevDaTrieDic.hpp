#ifndef PREVIOUS_PREV_DA_TRIE_DIC_HPP
#define PREVIOUS_PREV_DA_TRIE_DIC_HPP

#include <Basic.hpp>

namespace cda_tries {
namespace previous_tries {

enum class dic_type {
  CDA, DALF
};

class PrevDaTrieDic {
public:
  static std::unique_ptr<PrevDaTrieDic> create(dic_type type);

  PrevDaTrieDic() {}
  virtual ~PrevDaTrieDic() {}

  virtual void build(const std::vector<std::string> &strs) = 0;

  virtual uint32_t lookup(const char *str) const = 0;
  virtual void enumerate(std::vector<std::string> &ret) const = 0;

  virtual size_t num_strs() const = 0;
  virtual size_t bc_size() const = 0;
  virtual size_t num_emps() const = 0;
  virtual size_t tail_size() const = 0;
  virtual size_t size_in_bytes() const = 0;
  virtual void stat(std::ostream &os) const = 0;

  virtual void write(std::ostream &os) const = 0;
  virtual void read(std::istream &is) = 0;

  virtual void clear() = 0;
};

} // previous_tries
} // cda_tries

#endif //PREVIOUS_PREV_DA_TRIE_DIC_HPP
