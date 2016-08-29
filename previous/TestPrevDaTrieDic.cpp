#include <algorithm>
#include <cassert>
#include <random>
#include <fstream>

#include "PrevDaTrieDic.hpp"

namespace {

constexpr size_t NUM_STRS = 1U << 16;
constexpr size_t LENGTH   = 100;

void MakeStrs(std::vector<std::string> &strs) {
  strs.clear();
  strs.reserve(NUM_STRS);

  std::random_device rnd;
  auto make_char = [&]() {
    return (rnd() & 1 ? 'A' : 'a') + (rnd() % 26);
  };
  auto make_str = [&]() {
    std::string str;
    size_t length = (rnd() % LENGTH) + 1;
    for (size_t i = 0; i < length; ++i) {
      str += make_char();
    }
    return str;
  };

  for (size_t i = 0; i < NUM_STRS; ++i) {
    strs.push_back(make_str());
  }

  std::sort(strs.begin(), strs.end());
  strs.erase(std::unique(strs.begin(), strs.end()), strs.end());
}

void TestTrieDic(const std::vector<std::string> &strs, cda_tries::previous::dic_type type) {
  auto dic = cda_tries::previous::PrevDaTrieDic::create(type);
  dic->build(strs);

  assert(dic->num_strs() == strs.size());

  for (size_t i = 0; i < strs.size(); ++i) {
    assert(dic->lookup(strs[i].c_str()) != cda_tries::NOT_FOUND);
  }

  std::vector<std::string> ret;
  dic->enumerate(ret);

  assert(ret.size() == strs.size());
  for (size_t i = 0; i < strs.size(); ++i) {
    assert(ret[i] == strs[i]);
  }

  dic->clear();
  assert(dic->num_strs() == 0);
  assert(dic->bc_size() == 0);
  assert(dic->num_emps() == 0);
  assert(dic->tail_size() == 0);
}

}

int main() {
  std::vector<std::string> strs;
  MakeStrs(strs);

  TestTrieDic(strs, cda_tries::previous::dic_type::CDA);
  TestTrieDic(strs, cda_tries::previous::dic_type::DALF);

  return 0;
}
