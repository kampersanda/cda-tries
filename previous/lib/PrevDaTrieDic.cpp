#include "PrevDaTrieDic.hpp"
#include "CdaTrieDic.hpp"
#include "DalfTrieDic.hpp"

namespace cda_tries {
namespace previous {

std::unique_ptr<PrevDaTrieDic> PrevDaTrieDic::create(dic_type type) {
  std::unique_ptr<PrevDaTrieDic> ret;
  switch (type) {
    case dic_type::CDA:
      ret.reset(new CdaTrieDic);
      break;
    case dic_type::DALF:
      ret.reset(new DalfTrieDic);
      break;
  }
  return ret;
}

} // previous
} // cda_tries
