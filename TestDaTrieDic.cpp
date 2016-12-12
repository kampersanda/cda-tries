#undef NDEBUG

#include <algorithm>
#include <cassert>
#include <random>

#include <DaTrieDic.hpp>

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

void TestDaTrieDic(cda_tries::bc_type type, const std::vector<std::string> &strs) {
  cda_tries::DaTrieDic dic;
  dic.build(strs, type);

  assert(dic.num_strs() == strs.size());

  for (size_t i = 0; i < strs.size(); ++i) {
    auto str_id = dic.lookup(strs[i].c_str());
    assert(str_id != cda_tries::NOT_FOUND);
    std::string ret;
    dic.access(str_id, ret);
    assert(!ret.empty());
  }

  std::vector<uint32_t> ids;
  dic.enumerate(ids);

  assert(ids.size() == strs.size());

  for (size_t i = 0; i < strs.size(); ++i) {
    std::string ret;
    dic.access(ids[i], ret);
    assert(ret == strs[i]);
  }

  dic.clear();
  assert(dic.bc_size() == 0);
  assert(dic.tail_size() == 0);
  assert(dic.size_in_bytes() == 512);
  assert(dic.num_strs() == 0);
  assert(dic.max_length() == 0);
}

} // namespace

int main() {
  std::vector<std::string> strs;
  MakeStrs(strs);

  TestDaTrieDic(cda_tries::bc_type::PLAIN, strs);
  TestDaTrieDic(cda_tries::bc_type::DAC, strs);
  TestDaTrieDic(cda_tries::bc_type::FDAC, strs);

  return 0;
}
