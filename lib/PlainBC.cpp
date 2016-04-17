#include "PlainBC.hpp"

using namespace std;

namespace cda_tries {

PlainBC::PlainBC(const vector<bc_t> &bc, const vector<bool> &leafs) {
  bc_.Init(bc.size());
  for (uint i = 0; i < bc.size(); ++i) {
    bc_[i].base = bc[i].base << 1 | (leafs[i] ? 1 : 0);
    bc_[i].check = bc[i].check;
  }
}

void PlainBC::Stat(ostream &os) const {
  os << "PlainBC Stat..." << endl;
  os << "length    \t" << Length() << endl;
  os << "alloc size\t" << AllocSize() << endl;
  os << "elem size \t" << sizeof(bc_t) << endl;
}

} //cda_tries
