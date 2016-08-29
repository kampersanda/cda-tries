#ifndef CDA_TRIES_CODE_TABLE_HPP
#define CDA_TRIES_CODE_TABLE_HPP

#include "Basic.hpp"

namespace cda_tries {

class CodeTable {
public:
  CodeTable();
  ~CodeTable();

  size_t build(const std::vector<std::string> &strs);

  uint8_t code(uint8_t label) const {
    return table_[label];
  }
  uint8_t label(uint8_t code) const {
    return table_[code + 256];
  }

  size_t size() const;
  size_t size_in_bytes() const;

  void write(std::ostream &os) const;
  void read(std::istream &is);

  void clear();

  CodeTable(const CodeTable &) = delete;
  CodeTable &operator=(const CodeTable &) = delete;

private:
  uint8_t table_[512] = {};
};

} // cda_tries

#endif // CDA_TRIES_CODE_TABLE_HPP
