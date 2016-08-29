#include <cassert>
#include <random>

#include <Array.hpp>
#include <BitArray.hpp>
#include <SmallArray.hpp>

namespace {

constexpr uint32_t SIZE = 1U << 16;

void TestArray() {
  std::vector<uint32_t> orig_array;
  cda_tries::Array<uint32_t> array;

  std::random_device rnd;
  for (uint32_t i = 0; i < SIZE; ++i) {
    orig_array.push_back(rnd());
  }

  array.build(orig_array);
  assert(array.size() == orig_array.size());

  for (uint32_t i = 0; i < SIZE; ++i) {
    assert(array[i] == orig_array[i]);
  }
}

void TestBitArray() {
  std::vector<bool> orig_array;
  cda_tries::BitArray bit_array;

  std::random_device rnd;
  for (uint32_t i = 0; i < SIZE; ++i) {
    orig_array.push_back(rnd() % 2 == 0);
  }

  bit_array.build(orig_array);
  assert(bit_array.size() == orig_array.size());

  uint32_t sum = 0;
  for (uint32_t i = 0; i < SIZE; ++i) {
    assert(bit_array[i] == orig_array[i]);
    if (bit_array[i]) {
      assert(sum == bit_array.rank(i));
      assert(i == bit_array.select(sum));
      ++sum;
    }
  }

  assert(bit_array.num_1s() == sum);
  assert(bit_array.num_0s() == SIZE - sum);
}

void TestSmallArray() {
  std::vector<uint32_t> orig_array;
  cda_tries::SmallArray small_array;

  std::random_device rnd;
  for (uint32_t i = 0; i < SIZE; ++i) {
    orig_array.push_back(rnd() & UINT16_MAX);
  }

  small_array.build(orig_array);
  assert(small_array.size() == orig_array.size());

  for (uint32_t i = 0; i < SIZE; ++i) {
    assert(small_array[i] == orig_array[i]);
  }
}

} // namespace

int main() {
  TestArray();
  TestBitArray();
  TestSmallArray();

  return 0;
}
