#ifndef CDA_TRIES_BASIC_HPP
#define CDA_TRIES_BASIC_HPP

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include <ctime>

namespace cda_tries {

enum class bc_e { PLAIN, DAC, FDAC };

struct bc_t {
  uint32_t base;
  uint32_t check;
};

enum class sw_e { SEC, MILLI, MICRO };

class StopWatch {
public:
  StopWatch() : cl_(std::clock()) {}

  double Get(sw_e e) const {
    double sec = 1.0 * (std::clock() - cl_) / CLOCKS_PER_SEC;
    switch (e) {
      case sw_e::SEC:
        break;
      case sw_e::MILLI:
        sec *= 1000.0;
        break;
      case sw_e::MICRO:
        sec *= 1000000.0;
        break;
    }
    return sec;
  }

  StopWatch(const StopWatch &) = delete;
  StopWatch &operator=(const StopWatch &) = delete;

private:
  std::clock_t cl_;
};

} //cda_tries

#endif //CDA_TRIES_BASIC_HPP
