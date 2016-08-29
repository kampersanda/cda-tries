#include "Bc.hpp"
#include "DacBc.hpp"
#include "FastDacBc.hpp"
#include "PlainBc.hpp"

namespace cda_tries {

std::unique_ptr<Bc> Bc::create(bc_type type) {
  std::unique_ptr<Bc> ret;
  switch (type) {
    case bc_type::PLAIN:
      ret.reset(new PlainBc);
      break;
    case bc_type::DAC:
      ret.reset(new DacBc);
      break;
    case bc_type::FDAC:
      ret.reset(new FastDacBc);
      break;
  }
  return ret;
}

Bc::~Bc() {}

} // cda_tries
