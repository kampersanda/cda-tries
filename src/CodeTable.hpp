#ifndef CDA_TRIES_CODETABLE_HPP
#define CDA_TRIES_CODETABLE_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "Basic.hpp"

namespace cda_tries {

class CodeTable {
public:
  CodeTable();
  ~CodeTable() {}

  size_t Build(const std::vector<std::string> &strs);

  const uint8_t &operator[](uint8_t c) const { return table_[c]; }
  uint8_t Char(uint8_t code) const { return table_[code + 256]; }

  size_t AllocSize() const { return sizeof(table_); }

  void Save(std::ostream &os) const;
  void Load(std::istream &is);

  CodeTable(const CodeTable &) = delete;
  CodeTable &operator=(const CodeTable &) = delete;

private:
  uint8_t table_[512];
};

} //cda_tries

#endif //CDA_TRIES_CODETABLE_HPP
