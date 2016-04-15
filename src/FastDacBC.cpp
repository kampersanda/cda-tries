#include "FastDacBC.hpp"

using namespace std;

namespace cda_tries {

FastDacBC::FastDacBC(istream &is) {
  first_.Load(is);
  second_.Load(is);
  last_.Load(is);
  ranks_[0].Load(is);
  ranks_[1].Load(is);
  leafs_.Load(is);
  links_.Load(is);
}

FastDacBC::FastDacBC(const vector<bc_t> &bc, const vector<bool> &leafs) {
  if (bc.empty()) {
    return;
  }

  vector<uint8_t> first;
  vector<uint16_t> second;
  vector<uint32_t> last;
  vector<uint32_t> ranks[2];
  vector<uint32_t> links;

  first.reserve(bc.size() * 2);
  ranks[0].reserve((bc.size() * 2) >> 7);

  auto build_f = [&](uint val, bool isEmbed) {
    if ((first.size() % BLK1_LEN) == 0) {
      ranks[0].push_back(static_cast<uint32_t>(second.size()));
    }

    if (isEmbed) {
      first.push_back(val % 256);
      links.push_back(val >> 8);
      return;
    }

    if ((val / BLK1_LEN) == 0) {
      first.push_back(static_cast<uint8_t>(0 | (val << 1)));
      return;
    } else {
      size_t pos = second.size() - ranks[0].back();
      first.push_back(static_cast<uint8_t>(1 | (pos << 1)));
    }

    if ((second.size() % BLK2_LEN) == 0) {
      ranks[1].push_back(static_cast<uint32_t>(last.size()));
    }

    if ((val / BLK2_LEN) == 0) {
      second.push_back(static_cast<uint16_t>(0 | (val << 1)));
      return;
    } else {
      size_t pos = last.size() - ranks[1].back();
      second.push_back(static_cast<uint16_t>(1 | (pos << 1)));
    }

    last.push_back(val);
  };

  for (uint i = 0; i < bc.size(); ++i) {
    if (leafs[i]) {
      build_f(bc[i].base, true);
    } else {
      build_f(bc[i].base ^ i, false);
    }
    build_f(bc[i].check ^ i, false);
  }

  first_.Build(first);
  second_.Build(second);
  last_.Build(last);
  ranks_[0].Build(ranks[0]);
  ranks_[1].Build(ranks[1]);
  leafs_.Build(leafs);
  links_.Build(links);
}

size_t FastDacBC::Length() const {
  return first_.Length() / 2;
}

size_t FastDacBC::AllocSize() const {
  size_t size = DacAllocSize();
  size += leafs_.AllocSize();
  size += links_.AllocSize();
  return size;
}

void FastDacBC::Stat(ostream &os) const {
  size_t allocSize = AllocSize();
  os << "FastDacBC Stat..." << endl;
  os << "length    \t" << Length() << endl;
  os << "alloc size\t" << AllocSize() << endl;
  os << "elem size \t" <<
    static_cast<double>(allocSize) / Length() << endl;
  os << "1st ratio \t" <<
    static_cast<double>(first_.Length() - second_.Length()) / first_.Length() << endl;
  os << "link len  \t" << links_.Length() << endl;
  os << "link size \t" << links_.BitsPerElem() << endl;

  size_t dacAllocSize = DacAllocSize();
  os << "Alloc Detail..." << endl;
  os << "fdac \t" << dacAllocSize << "\t" <<
    static_cast<double>(dacAllocSize) / allocSize << endl;
  os << "leafs\t" << leafs_.AllocSize() << "\t" <<
    static_cast<double>(leafs_.AllocSize()) / allocSize << endl;
  os << "links\t" << links_.AllocSize() << "\t" <<
    static_cast<double>(links_.AllocSize()) / allocSize << endl;
}

void FastDacBC::Save(ostream &os) const {
  first_.Save(os);
  second_.Save(os);
  last_.Save(os);
  ranks_[0].Save(os);
  ranks_[1].Save(os);
  leafs_.Save(os);
  links_.Save(os);
}

size_t FastDacBC::DacAllocSize() const {
  size_t size = 0;
  size += first_.AllocSize();
  size += second_.AllocSize();
  size += last_.AllocSize();
  size += ranks_[0].AllocSize();
  size += ranks_[1].AllocSize();
  return size;
}

} //cda_tries
