#include <algorithm>

#include "DaTrie.hpp"
#include "Builder.hpp"

using namespace std;

namespace cda_tries {

DaTrie::DaTrie(istream &is, bc_e type) {
  is.read(reinterpret_cast<char *>(&numStrs_), sizeof(numStrs_));
  is.read(reinterpret_cast<char *>(&maxLen_), sizeof(maxLen_));
  table_.Load(is);
  bc_ = BC::Load(is, type);
  terms_.Load(is);
  tail_.Load(is);
}

DaTrie::DaTrie(const vector<string> &strs, bc_e type) {
  numStrs_ = strs.size();
  maxLen_ = table_.Build(strs);
  Builder builder(strs, table_, type);
  bc_ = BC::Build(builder.bc_, builder.leafs_, type);
  terms_.Build(builder.terms_);
  tail_.Build(builder.tail_);
}

uint DaTrie::Lookup(const char *str) const {
  uint idx = 0;
  uint next = bc_->Base(idx) ^ table_[*str++];

  while (!bc_->IsLeaf(next)) {
    if (bc_->Check(next) != idx) {
      return NOT_FOUND;
    } else if (!*str) {
      return terms_[next] ? IdxToId(next) : NOT_FOUND;
    }
    idx = next;
    next = bc_->Base(idx) ^ table_[*str++];
  }

  const char *tail = &tail_[bc_->Link(next)];
  while (*str && *str == *tail) {
    str++, tail++;
  }
  return *str == *tail ? IdxToId(next) : NOT_FOUND;
}

void DaTrie::Access(uint id, string &str) const {
  str.clear();

  if (numStrs_ <= id) {
    return;
  }

  str.reserve(maxLen_);

  uint idx = IdToIdx(id);
  const uint link = bc_->IsLeaf(idx) ? bc_->Link(idx) : NOT_FOUND;

  while (idx != 0) {
    const uint prev = bc_->Check(idx);
    str += table_.Char(bc_->Base(prev) ^ idx);
    idx = prev;
  }

  reverse(str.begin(), str.end());
  if (link != NOT_FOUND) {
    str += &tail_[link];
  }
}

size_t DaTrie::NumElems() const {
  return bc_->Length();
}

size_t DaTrie::AllocSize() const {
  size_t size = 0;
  size += table_.AllocSize();
  size += bc_->AllocSize();
  size += terms_.AllocSize();
  size += tail_.AllocSize();
  return size;
}

void DaTrie::Stat(ostream &os) const {
  const size_t allocSize = AllocSize();

  os << "Double-array Trie Stat..." << endl;
  os << "bc len    \t" << NumElems() << endl;
  os << "tail len  \t" << tail_.Length() << endl;
  os << "alloc size\t" << allocSize << endl;

  os << "Alloc Detail..." << endl;
  os << "bc   \t" << bc_->AllocSize() << "\t" <<
    static_cast<double>(bc_->AllocSize()) / allocSize << endl;
  os << "terms\t" << terms_.AllocSize() << "\t" <<
    static_cast<double>(terms_.AllocSize()) / allocSize << endl;
  os << "tail \t" << tail_.AllocSize() << "\t" <<
    static_cast<double>(tail_.AllocSize()) / allocSize << endl;
  os << "table\t" << table_.AllocSize() << endl;

  bc_->Stat(os);
}

void DaTrie::Save(ostream &os) const {
  os.write(reinterpret_cast<const char *>(&numStrs_), sizeof(numStrs_));
  os.write(reinterpret_cast<const char *>(&maxLen_), sizeof(maxLen_));
  table_.Save(os);
  bc_->Save(os);
  terms_.Save(os);
  tail_.Save(os);
}

} //cda_tries
