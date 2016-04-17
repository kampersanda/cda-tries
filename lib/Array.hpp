#ifndef CDA_TRIES_ARRAY_HPP
#define CDA_TRIES_ARRAY_HPP

#include <fstream>
#include <memory>
#include <vector>

#include "Basic.hpp"

namespace cda_tries {

template <typename T>
class Array {
public:
  Array() {}
  ~Array() {}

  void Init(size_t len) {
    len_ = len;
    array_.reset(new T[len_]);
  }

  void Init(size_t len, T init) {
    Init(len);
    for (size_t i = 0; i < len_; ++i) {
      array_[i] = init;
    }
  }

  void Build(const std::vector<T> &src) {
    Init(src.size());
    for (size_t i = 0; i < len_; ++i) {
      array_[i] = src[i];
    }
  }

  const T &operator[](size_t idx) const { return array_[idx]; }
  T &operator[](size_t idx) { return array_[idx]; }

  bool IsEmpty() const { return len_ == 0; }
  size_t Length() const { return len_; }
  size_t AllocSize() const { return sizeof(T) * len_; }

  void Save(std::ostream &os) const {
    os.write(reinterpret_cast<const char *>(&len_), sizeof(len_));
    os.write(reinterpret_cast<const char *>(&array_[0]), sizeof(array_[0]) * len_);
  }

  void Load(std::istream &is) {
    Clear();
    is.read(reinterpret_cast<char *>(&len_), sizeof(len_));
    array_.reset(new T[len_]);
    is.read(reinterpret_cast<char *>(&array_[0]), sizeof(array_[0]) * len_);
  }

  void Clear() {
    array_.reset();
    len_ = 0;
  }

  Array(const Array &) = delete;
  Array &operator=(const Array &) = delete;

private:
  std::unique_ptr<T[]> array_;
  size_t len_ = 0;
};

} //cda_tries

#endif //CDA_TRIES_ARRAY_HPP
