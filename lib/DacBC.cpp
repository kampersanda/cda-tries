#include "DacBC.hpp"

using namespace std;

namespace cda_tries {

DacBC::DacBC(istream &is) {
  is.read(reinterpret_cast<char *>(&numLevels_), sizeof(numLevels_));
  for (uint j = 0; j < numLevels_ - 1; ++j) {
    bytes_[j].Load(is);
    bits_[j].Load(is);
  }
  bytes_[numLevels_ - 1].Load(is);
  leafs_.Load(is);
  links_.Load(is);
}

DacBC::DacBC(const vector<bc_t> &bc, const vector<bool> &leafs) {
  if (bc.empty()) {
    return;
  }

  numLevels_ = 1;
  vector<uint8_t> bytes[4];
  vector<bool> bits[4];
  vector<uint> links;

  bytes[0].reserve(bc.size() * 2);
  bits[0].reserve(bc.size() * 2);

  auto build_f = [&](uint val, bool isEmbed) {
    if (isEmbed) {
      links.push_back(val >> 8);
      bytes[0].push_back(val % 256);
      bits[0].push_back(false);
      return;
    }

    uint level = 0;
    bytes[level].push_back(static_cast<uint8_t>(val & 0xFF));
    bits[level].push_back(true);
    val >>= 8;

    while (val) {
      level++;
      bytes[level].push_back(static_cast<uint8_t>(val & 0xFF));
      bits[level].push_back(true);
      val >>= 8;
    }

    bits[level].back() = false;
    numLevels_ = max(numLevels_, level + 1);
  };

  for (uint i = 0; i < bc.size(); ++i) {
    if (leafs[i]) {
      build_f(bc[i].base, true);
    } else {
      build_f(bc[i].base ^ i, false);
    }
    build_f(bc[i].check ^ i, false);
  }

  for (uint j = 0; j < numLevels_ - 1; ++j) {
    bytes_[j].Build(bytes[j]);
    bits_[j].Build(bits[j]);
  }
  bytes_[numLevels_ - 1].Build(bytes[numLevels_ - 1]);
  leafs_.Build(leafs);
  links_.Build(links);
}

size_t DacBC::Length() const {
  return bytes_[0].Length() / 2;
}

size_t DacBC::AllocSize() const {
  size_t size = DacAllocSize();
  size += leafs_.AllocSize();
  size += links_.AllocSize();
  return size;
}

void DacBC::Stat(ostream &os) const {
  size_t allocSize = AllocSize();
  os << "DacBC Stat..." << endl;
  os << "length    \t" << Length() << endl;
  os << "alloc size\t" << allocSize << endl;
  os << "elem size \t" <<
    static_cast<double>(allocSize) / Length() << endl;
  os << "1st ratio \t" <<
    static_cast<double>(bytes_[0].Length() - bytes_[1].Length()) / bytes_[0].Length() << endl;
  os << "link len  \t" << links_.Length() << endl;
  os << "link size \t" << links_.BitsPerElem() << endl;

  size_t dacAllocSize = DacAllocSize();
  os << "Alloc Detail..." << endl;
  os << "dac  \t" << dacAllocSize << "\t" <<
    static_cast<double>(dacAllocSize) / allocSize << endl;
  os << "leafs\t" << leafs_.AllocSize() << "\t" <<
    static_cast<double>(leafs_.AllocSize()) / allocSize << endl;
  os << "links\t" << links_.AllocSize() << "\t" <<
    static_cast<double>(links_.AllocSize()) / allocSize << endl;
}

void DacBC::Save(ostream &os) const {
  os.write(reinterpret_cast<const char *>(&numLevels_), sizeof(numLevels_));
  for (uint j = 0; j < numLevels_ - 1; ++j) {
    bytes_[j].Save(os);
    bits_[j].Save(os);
  }
  bytes_[numLevels_ - 1].Save(os);
  leafs_.Save(os);
  links_.Save(os);
}

size_t DacBC::DacAllocSize() const {
  size_t size = 0;
  for (uint j = 0; j < numLevels_ - 1; ++j) {
    size += bytes_[j].AllocSize();
    size += bits_[j].AllocSize();
  }
  size += bytes_[numLevels_ - 1].AllocSize();
  return size;
}

} //cda_tries
