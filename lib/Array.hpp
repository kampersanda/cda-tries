#ifndef CDA_TRIES_ARRAY_HPP
#define CDA_TRIES_ARRAY_HPP

#include "Basic.hpp"

namespace cda_tries {

template <class T>
class Array {
public:
  Array() {}
  ~Array() {}

  void reset(size_t size) {
    size_ = size;
    array_.reset(new T[size_]);
  }

  void reset(size_t size, const T &initial) {
    reset(size);
    for (size_t i = 0; i < size_; ++i) {
      array_[i] = initial;
    }
  }

  void build(const std::vector<T> &array) {
    reset(array.size());
    for (size_t i = 0; i < size_; ++i) {
      array_[i] = array[i];
    }
  }

  const T &operator[](size_t pos) const {
    return array_[pos];
  }
  T &operator[](size_t pos) {
    return array_[pos];
  }

  size_t size() const {
    return size_;
  }
  size_t size_in_bytes() const {
    return sizeof(T) * size_;
  }

  bool is_empty() const {
    return size_ == 0;
  }

  void write(std::ostream &os) const {
    os.write(reinterpret_cast<const char *>(&size_), sizeof(size_));
    os.write(reinterpret_cast<const char *>(&array_[0]), sizeof(array_[0]) * size_);
  }

  void read(std::istream &is) {
    clear();
    is.read(reinterpret_cast<char *>(&size_), sizeof(size_));
    array_.reset(new T[size_]);
    is.read(reinterpret_cast<char *>(&array_[0]), sizeof(array_[0]) * size_);
  }

  void clear() {
    array_.reset();
    size_ = 0;
  }

  Array(const Array &) = delete;
  Array &operator=(const Array &) = delete;

private:
  std::unique_ptr<T[]> array_;
  size_t size_ = 0;
};

} // cda_tries

#endif // CDA_TRIES_ARRAY_HPP
