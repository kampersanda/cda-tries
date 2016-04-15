#include "BitArray.hpp"

using namespace std;

namespace cda_tries {

void BitArray::Build(const vector<bool> &bits) {
  numBits_ = bits.size();
  num1s_ = 0;

  bits_.Init(numBits_ / 32 + 1, 0);
  rank1st_.Init(numBits_ / R1_BLK_LEN + 1, 0);
  rank2nd_.Init(numBits_ / R2_BLK_LEN + 1, 0);

  for (uint i = 0; i <= numBits_; ++i) {
    if ((i % R1_BLK_LEN) == 0) {
      rank1st_[i / R1_BLK_LEN] = num1s_;
    }
    if ((i % R2_BLK_LEN) == 0) {
      rank2nd_[i / R2_BLK_LEN] = num1s_ - rank1st_[i / R1_BLK_LEN];
    }
    if (bits[i]) {
      bits_[i / 32] |= (1U << (i % 32));
      num1s_++;
    }
  }
}

uint BitArray::Select(uint cnt) const {
  cnt++;

  uint left  = 0;
  uint right = rank1st_.Length();

  while (left < right){
    uint mid = (left + right) / 2;
    if (rank1st_[mid] < cnt) {
      left  = mid + 1;
    } else {
      right = mid;
    }
  }

  uint pos1st = (left != 0) ? left - 1 : 0;
  uint pos2nd = pos1st * BLKS_PER_1ST;

  cnt -= rank1st_[pos1st];
  pos2nd++;
  while ((pos2nd < rank2nd_.Length())
         && ((pos2nd % BLKS_PER_1ST) != 0)
         && (rank2nd_[pos2nd] < cnt)){
    pos2nd++;
  }
  pos2nd--;

  cnt -= rank2nd_[pos2nd];
  uint ret = pos2nd * R2_BLK_LEN;
  uint posBits = pos2nd * (R2_BLK_LEN / 32);
  uint rank32 = PopCount(bits_[posBits]);

  if (rank32 < cnt){
    posBits++;
    ret += 32;
    cnt -= rank32;
  }

  posBits = bits_[posBits];
  uint rank16 = PopCount(posBits & ((1U << 16) - 1));
  if (rank16 < cnt){
    posBits >>= 16;
    ret += 16;
    cnt -= rank16;
  }

  uint rank8  = PopCount(posBits & ((1U << 8) - 1));
  if (rank8 < cnt){
    posBits >>= 8;
    ret += 8;
    cnt -= rank8;
  }

  while (cnt > 0){
    if (posBits & 1){
      cnt--;
    }
    posBits >>= 1;
    ret++;
  }

  return ret - 1;
}

size_t BitArray::NumBits() const {
  return numBits_;
}

size_t BitArray::Num1s() const {
  return num1s_;
}

size_t BitArray::AllocSize() const {
  return bits_.AllocSize() + rank1st_.AllocSize() + rank2nd_.AllocSize();
}

void BitArray::Save(ostream &os) const {
  bits_.Save(os);
  rank1st_.Save(os);
  rank2nd_.Save(os);
  os.write(reinterpret_cast<const char *>(&numBits_), sizeof(numBits_));
  os.write(reinterpret_cast<const char *>(&num1s_), sizeof(num1s_));
}

void BitArray::Load(istream &is) {
  bits_.Load(is);
  rank1st_.Load(is);
  rank2nd_.Load(is);
  is.read(reinterpret_cast<char *>(&numBits_), sizeof(numBits_));
  is.read(reinterpret_cast<char *>(&num1s_), sizeof(num1s_));
}

} //cda_tries
