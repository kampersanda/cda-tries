#include <iostream>
#include <fstream>

#include "PrevDaTrieDic.hpp"

using namespace cda_tries;
using namespace cda_tries::previous_tries;

namespace {

constexpr size_t RUNS = 10;

bool Check(const std::unique_ptr<PrevDaTrieDic> &dic, const std::vector<std::string> &strs) {
  auto num_strs = strs.size();
  std::cout << "Checking for " << num_strs << " strs..." << std::endl;

  for (size_t i = 0; i < num_strs; ++i) {
    auto str_id = dic->lookup(strs[i].c_str());
    if (str_id == NOT_FOUND) {
      std::cerr << "Error: failed to lookup " << strs[i].c_str() << std::endl;
      return false;
    }
  }

  std::cout << "-> No error" << std::endl;
  return true;
}

void BenchmarkLookup(const std::unique_ptr<PrevDaTrieDic> &dic, const std::vector<std::string> &strs) {
  auto num_strs = strs.size();
  std::cout << "Lookup benchmark on " << RUNS << " runs" << std::endl;

  StopWatch sw;
  for (size_t i = 0; i < RUNS; ++i) {
    for (size_t j = 0; j < num_strs; ++j) {
      dic->lookup(strs[j].c_str());
    }
  }

  auto ave_time = sw.get(sw_type::MICRO) / RUNS;
  std::cout << "-> Time: " << ave_time / num_strs << " us per str" << std::endl;
}

void ShowUsage(std::ostream &os) {
  os << "Benchmark <mode> <type> <str_path> <dic_path>" << std::endl;
  os << "  <mode> Running mode" << std::endl;
  os << "         1: Build, 2: Benchmark" << std::endl;
  os << "  <type> CDA type" << std::endl;
  os << "         1: CDA, 2: DALF" << std::endl;
  os << "  <str_path> File path of strings" << std::endl;
  os << "  <dic_path> File path of dictionary" << std::endl;
}

} // namespace

int main(int argc, const char *argv[]) {
  if (argc != 5) {
    ShowUsage(std::cout);
    return 1;
  }

  auto mode = *argv[1];
  if (mode != '1' && mode != '2') {
    ShowUsage(std::cout);
    return 1;
  }

  dic_type type;
  switch (*argv[2]) {
    case '1':
      type = dic_type::CDA;
      break;
    case '2':
      type = dic_type::DALF;
      break;
    default:
      ShowUsage(std::cout);
      return 1;
  }

  std::string str_path(argv[3]);
  std::string dic_path(argv[4]);

  auto dic = PrevDaTrieDic::create(type);
  std::vector<std::string> strs;

  {
    std::ifstream ifs(str_path);
    if (!ifs) {
      std::cerr << "Error: failed to open strings: " << str_path << std::endl;
      return 1;
    }
    LoadStrings(ifs, strs);
  }

  // Build
  if (mode == '1') {
    StopWatch sw;
    dic->build(strs);
    std::cout << "Constr. time: " << sw.get(sw_type::SEC) << " sec" << std::endl;

    std::ofstream ofs(dic_path, std::ios::binary);
    if (!ofs) {
      std::cerr << "Error: failed to write " << dic_path << std::endl;
      return 1;
    }
    dic->write(ofs);
  }

  // Benchmark
  if (mode == '2') {
    std::ifstream ifs(dic_path, std::ios::binary);
    if (!ifs) {
      std::cerr << "Error: failed to read " << dic_path << std::endl;
      return 1;
    }

    dic->read(ifs);
    dic->status(std::cout);
    std::cout << std::endl;

    if (!Check(dic, strs)) {
      return -1;
    }
    BenchmarkLookup(dic, strs);
  }

  return 0;
}
