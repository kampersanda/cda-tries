#include "BC.hpp"
#include "DacBC.hpp"
#include "FastDacBC.hpp"
#include "PlainBC.hpp"

using namespace std;

namespace cda_tries {

unique_ptr<BC> BC::Load(istream &is, bc_e type) {
  unique_ptr<BC> ret;
  switch (type) {
    case bc_e::PLAIN:
      ret.reset(new PlainBC(is));
      break;
    case bc_e::DAC:
      ret.reset(new DacBC(is));
      break;
    case bc_e::FDAC:
      ret.reset(new FastDacBC(is));
      break;
  }
  return ret;
}

unique_ptr<BC> BC::Build(const vector<bc_t> &bc, const vector<bool> &leafs, bc_e type) {
  unique_ptr<BC> ret;
  switch (type) {
    case bc_e::PLAIN:
      ret.reset(new PlainBC(bc, leafs));
      break;
    case bc_e::DAC:
      ret.reset(new DacBC(bc, leafs));
      break;
    case bc_e::FDAC:
      ret.reset(new FastDacBC(bc, leafs));
      break;
  }
  return ret;
}

} //cda_tries
