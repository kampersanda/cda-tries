#include <algorithm>
#include <fstream>

#include "CodeTable.hpp"

using namespace std;

namespace cda_tries {

CodeTable::CodeTable() {
  for (uint c = 0; c < 256; ++c) {
    table_[c] = c;
    table_[c + 256] = c;
  }
}

size_t CodeTable::Build(const vector<string> &strs) {
  if (strs.empty()) {
    return 0;
  }

  struct freq_t {
    size_t freq = 0;
    uint8_t c = 0;
  };
  vector<freq_t> freqs(256);

  for (uint c = 0; c < 256; ++c) {
    freqs[c].c = c;
  }

  size_t maxLen = 0;
  for (size_t i = 0; i < strs.size(); ++i) {
    for (const uint8_t c : strs[i]) {
      freqs[c].freq++;
    }
    maxLen = max(maxLen, strs[i].size());
  }

  sort(begin(freqs), end(freqs),
       [](const freq_t &lhs, const freq_t &rhs) {
         return lhs.freq > rhs.freq;
       });

  for (uint c = 0; c < 256; ++c) {
    table_[freqs[c].c] = c;
  }

  for (uint c = 0; c < 256; ++c) {
    table_[table_[c] + 256] = c;
  }

  return maxLen;
}

void CodeTable::Save(ostream &os) const {
  os.write(reinterpret_cast<const char *>(&table_[0]), sizeof(table_));
}

void CodeTable::Load(istream &is) {
  is.read(reinterpret_cast<char *>(&table_[0]), sizeof(table_));
}

} //cda_tries
