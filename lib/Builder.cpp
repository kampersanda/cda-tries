#include <algorithm>
#include <cassert>

#include "Builder.hpp"

using namespace std;

namespace cda_tries {

Builder::Builder(const vector<string> &strs, const CodeTable &table, bc_e type)
  : strs_(strs), table_(table), type_(type) {
  assert(!strs.empty());
  assert(strs.size() <= UPPER);

  switch (type_) {
    case bc_e::PLAIN:
      blkLen_ = 0;
      break;
    case bc_e::DAC:
      blkLen_ = 256;
      break;
    case bc_e::FDAC:
      blkLen_ = 128;
      break;
  }

  emptyHead_ = INV_IDX;
  edges_.reserve(256);
  suffixes_.reserve(strs_.size());

  Expand();
  Fix(0);
  Arrange(0, strs_.size(), 0, 0);
  DeleteEmptyList();
  UnifyTail();
}

void Builder::Expand() {
  assert(bc_.size() + 256 <= UPPER);

  const uint oldSize = bc_.size();
  const uint newSize = oldSize + 256;

  if (bc_.capacity() < newSize) {
    size_t capa;
    if (bc_.empty()) {
      capa = 1;
      while (capa < strs_.size()) {
        capa <<= 1;
      }
    } else {
      capa = bc_.capacity() << 1;
    }

    bc_.reserve(capa);
    leafs_.reserve(capa);
    terms_.reserve(capa);
    fixed_.reserve(capa);
    if (blkLen_ != 0) {
      heads_.reserve(capa / blkLen_);
    }
  }

  bc_.resize(newSize);
  leafs_.resize(newSize, false);
  terms_.resize(newSize, false);
  fixed_.resize(newSize, false);

  for (uint i = oldSize; i < newSize; ++i) {
    SetPrev(i, i - 1);
    SetNext(i, i + 1);
  }

  if (emptyHead_ == INV_IDX) {
    SetPrev(oldSize, newSize - 1);
    SetNext(newSize - 1, oldSize);
    emptyHead_ = oldSize;
  } else {
    const uint emptyTail = Prev(emptyHead_);
    SetPrev(oldSize, emptyTail);
    SetNext(emptyTail, oldSize);
    SetPrev(emptyHead_, newSize - 1);
    SetNext(newSize - 1, emptyHead_);
  }

  if (blkLen_ != 0) {
    for (uint i = oldSize; i < newSize; i += blkLen_) {
      heads_.push_back(i);
    }
  }

  const uint oldNumBlk = oldSize / 256;
  const uint newNumBlk = oldNumBlk + 1;

  if (newNumBlk > 16) {
    const uint blk = oldNumBlk - 16;
    const uint begin = blk * 256;
    const uint end = (blk + 1) * 256;

    for (uint i = begin; i < end; ++i) {
      Fix(i);
    }
  }
}

void Builder::Fix(uint idx) {
  if (fixed_[idx]) {
    return;
  }

  fixed_[idx] = true;
  const uint prev = Prev(idx);
  const uint next = Next(idx);
  SetPrev(next, prev);
  SetNext(prev, next);

  if (idx == emptyHead_) {
    emptyHead_ = idx == next ? INV_IDX : next;
  }

  if (blkLen_ != 0) {
    const uint blk = idx / blkLen_;
    if (idx == heads_[blk]) {
      if (emptyHead_ == INV_IDX || blk != next / blkLen_) {
        heads_[blk] = INV_IDX;
      } else {
        heads_[blk] = next;
      }
    }
  }

  bc_[idx] = bc_t{idx, idx};
}

void Builder::Arrange(size_t begin, size_t end, size_t level, uint idx) {
  if (strs_[begin].size() == level) {
    ++begin;
    terms_[idx] = true;
    if (begin == end) { // without link
      bc_[idx].base = 0;
      leafs_[idx] = true;
      return;
    }
  }

  if (begin + 1 == end && !terms_[idx]) { // leaf node
    assert(strs_[begin].size() != level);
    leafs_[idx] = true;
    terms_[idx] = true;
    suffix_t suf{strs_[begin].substr(level), idx};
    reverse(suf.rstr.begin(), suf.rstr.end());
    suffixes_.push_back(suf);
    return;
  }

  { // Fetch Edges
    edges_.clear();
    uint8_t label = strs_[begin][level];
    for (size_t id = begin + 1; id < end; ++id) {
      const uint8_t newLabel = strs_[id][level];
      if (label != newLabel) {
        edges_.push_back(label);
        assert(label <= newLabel);
        label = newLabel;
      }
    }
    edges_.push_back(label);
  }

  const uint base = blkLen_ == 0 ? XCheck() : YCheck(idx / blkLen_);
  if (bc_.size() <= base) {
    Expand();
  }

  // Define
  bc_[idx].base = base;
  for (const uint8_t label : edges_) {
    const uint next = base ^ table_[label];
    Fix(next);
    bc_[next].check = idx;
  }

  // Follow
  size_t newBegin = begin;
  uint8_t label = strs_[begin][level];
  for (size_t newEnd = begin + 1; newEnd < end; ++newEnd) {
    const uint8_t newLabel = strs_[newEnd][level];
    if (label != newLabel) {
      Arrange(newBegin, newEnd, level + 1, base ^ table_[label]);
      label = newLabel;
      newBegin = newEnd;
    }
  }
  Arrange(newBegin, end, level + 1, base ^ table_[label]);
}

void Builder::DeleteEmptyList() {
  if (emptyHead_ != INV_IDX) {
    uint idx = emptyHead_;
    do {
      const uint next = Next(idx);
      bc_[idx].base = idx;
      bc_[idx].check = idx;
      idx = next;
    } while (emptyHead_ != idx);
  }
}

void Builder::UnifyTail() {
  sort(suffixes_.begin(), suffixes_.end(),
       [](const suffix_t &lhs, const suffix_t &rhs){
         return lhs.rstr < rhs.rstr;
       });
  tail_.push_back('\0');

  auto append_f = [&](size_t begin, size_t end, const string &rstr) {
    for (auto rit = rstr.rbegin(); rit != rstr.rend(); ++rit) {
      tail_.push_back(*rit);
    }

    while (begin < end) {
      const suffix_t &suf = suffixes_[begin++];
      bc_[suf.idx].base = tail_.size() - suf.rstr.size();
    }

    tail_.push_back('\0');
  };

  size_t begin = 0;
  for (size_t i = 1; i < suffixes_.size(); ++i) {
    const string &obj = suffixes_[i - 1].rstr;
    const string &sbj = suffixes_[i].rstr;

    // can unifing
    if (obj.size() <= sbj.size()
        && obj.compare(0, obj.size(), sbj, 0, obj.size()) == 0) {
      continue;
    }

    append_f(begin, i, obj);
    begin = i;
  }

  append_f(begin, suffixes_.size(), suffixes_.back().rstr);
}

uint Builder::XCheck() const {
  if (emptyHead_ == INV_IDX) {
    return fixed_.size();
  }

  uint idx = emptyHead_;
  do {
    const uint base = idx ^ table_[edges_[0]];
    if (IsTarget(base)) {
      return base;
    }
    idx = Next(idx);
  } while (emptyHead_ != idx);

  return fixed_.size();
}

uint Builder::YCheck(uint blk) const {
  if (emptyHead_ == INV_IDX) {
    return fixed_.size();
  }

  uint idx = heads_[blk];
  if (idx != INV_IDX) {
    do {
      const uint base = idx ^ table_[edges_[0]];
      if (IsTarget(base)) {
        return base;
      }
      idx = Next(idx);
    } while (emptyHead_ != idx && idx / blkLen_ == blk);
  }

  return XCheck();
}

bool Builder::IsTarget(uint base) const {
  for (uint8_t label : edges_) {
    const uint idx = base ^ table_[label];
    if (fixed_[idx]) {
      return false;
    }
  }
  return true;
}

} //cda_tries
