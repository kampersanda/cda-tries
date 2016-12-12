#undef NDEBUG

#include <algorithm>
#include <cassert>
#include <random>

#include <Bc.hpp>

namespace {

constexpr size_t SIZE = 1U << 16;

void MakeBc(std::vector<cda_tries::bc_t> &bc) {
  std::random_device rnd;

  auto make_elem = [&]() {
    cda_tries::bc_t elem;
    switch (rnd() % 3) {
      case 0: // internal node
        elem.set_base(rnd() % cda_tries::BC_UPPER);
        elem.set_check(rnd() % cda_tries::BC_UPPER);
        elem.fix();
        break;
      case 1: // leaf node
        elem.set_link(rnd() % cda_tries::BC_UPPER);
        elem.set_check(rnd() % cda_tries::BC_UPPER);
        elem.fix();
        break;
      case 2: // invalid node
        elem.set_base(rnd() % cda_tries::BC_UPPER);
        elem.set_check(rnd() % cda_tries::BC_UPPER);
        break;
      default:
        break;
    }
    return elem;
  };

  bc.resize(SIZE);
  for (size_t i = 0; i < SIZE; ++i) {
    bc[i] = make_elem();
  }
}

void TestBc(cda_tries::bc_type type, const std::vector<cda_tries::bc_t> &orig_bc) {
  std::unique_ptr<cda_tries::Bc> bc;
  bc = cda_tries::Bc::create(type);

  bc->build(orig_bc);

  assert(bc->size() == orig_bc.size());
  for (uint32_t i = 0; i < orig_bc.size(); ++i) {
    assert(bc->is_fixed(i) == orig_bc[i].is_fixed());
    if (!bc->is_fixed(i)) {
      continue;
    }
    assert(bc->is_leaf(i) == orig_bc[i].is_leaf());
    assert(bc->check(i) == orig_bc[i].check());
    if (!bc->is_leaf(i)) {
      assert(bc->base(i) == orig_bc[i].base());
    } else {
      assert(bc->link(i) == orig_bc[i].link());
    }
  }

  bc->clear();
  assert(bc->size() == 0);
}

} // namespace

int main() {
  std::vector<cda_tries::bc_t> orig_bc;
  MakeBc(orig_bc);

  TestBc(cda_tries::bc_type::PLAIN, orig_bc);
  TestBc(cda_tries::bc_type::DAC, orig_bc);
  TestBc(cda_tries::bc_type::FDAC, orig_bc);

  return 0;
}
