#include <iostream>

#include <DaTrieDic.hpp>

using namespace cda_tries;

namespace {

constexpr size_t RUNS = 10;

bool Check(const DaTrieDic &dic, const std::vector<std::string> &strs, std::vector<uint32_t> &ids) {
  auto num_strs = strs.size();
  std::cout << "Checking for " << num_strs << " strs..." << std::endl;

  ids.clear();
  ids.reserve(num_strs);

  for (size_t i = 0; i < num_strs; ++i) {
    auto str_id = dic.lookup(strs[i].c_str());
    if (str_id == NOT_FOUND) {
      std::cerr << "Error: failed to lookup " << strs[i].c_str() << std::endl;
      return false;
    }

    std::string ret;
    dic.access(str_id, ret);
    if (ret.empty()) {
      std::cerr << "Error: failed to access " << str_id << std::endl;
      return false;
    }
    ids.push_back(str_id);
  }

  std::cout << "-> No error" << std::endl;
  return true;
}

void BenchmarkLookup(const DaTrieDic &dic, const std::vector<std::string> &strs) {
  auto num_strs = strs.size();
  std::cout << "Lookup benchmark on " << RUNS << " runs" << std::endl;

  StopWatch sw;
  for (size_t i = 0; i < RUNS; ++i) {
    for (size_t j = 0; j < num_strs; ++j) {
      dic.lookup(strs[j].c_str());
    }
  }

  auto ave_time = sw.get(sw_type::MICRO) / RUNS;
  std::cout << "-> Time: " << ave_time / num_strs << " us per str" << std::endl;
}

void BenchmarkAccess(const DaTrieDic &dic, const std::vector<uint32_t> &ids) {
  auto num_ids = ids.size();
  std::cout << "Access benchmark on " << RUNS << " runs" << std::endl;

  StopWatch sw;
  for (size_t i = 0; i < RUNS; ++i) {
    for (size_t j = 0; j < num_ids; ++j) {
      std::string ret;
      dic.access(ids[j], ret);
    }
  }

  auto ave_time = sw.get(sw_type::MICRO) / RUNS;
  std::cout << "-> Time: " << ave_time / num_ids << " us per ID" << std::endl;
}

void ShowUsage(std::ostream &os) {
  os << "Benchmark <mode> <type> <str_path> <dic_path>" << std::endl;
  os << "  <mode> Running mode" << std::endl;
  os << "         1: Build, 2: Benchmark" << std::endl;
  os << "  <type> Representation type of BASE and CHECK" << std::endl;
  os << "         1: Plain, 2: DACs, 3: Fast DACs" << std::endl;
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

  bc_type type;
  switch (*argv[2]) {
    case '1':
      type = bc_type::PLAIN;
      break;
    case '2':
      type = bc_type::DAC;
      break;
    case '3':
      type = bc_type::FDAC;
      break;
    default:
      ShowUsage(std::cout);
      return 1;
  }

  std::string str_path(argv[3]);
  std::string dic_path(argv[4]);

  DaTrieDic dic;
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
    if (type != bc_type::PLAIN) {
#ifdef ENABLE_YCHECK
      std::cout << "--> Enable YCHECK <--" << std::endl;
#else
      std::cout << "--> Disable YCHECK <--" << std::endl;
#endif
    }

    StopWatch sw;
    dic.build(strs, type);
    std::cout << "Constr. time: " << sw.get(sw_type::SEC) << " sec" << std::endl;

    std::ofstream ofs(dic_path, std::ios::binary);
    if (!ofs) {
      std::cerr << "Error: failed to write " << dic_path << std::endl;
      return 1;
    }
    dic.write(ofs);
  }

  // Benchmark
  if (mode == '2') {
    std::ifstream ifs(dic_path, std::ios::binary);
    if (!ifs) {
      std::cerr << "Error: failed to read " << dic_path << std::endl;
      return 1;
    }

    dic.read(ifs, type);
    dic.stat(std::cout);
    std::cout << std::endl;

    std::vector<uint32_t> ids;
    if (!Check(dic, strs, ids)) {
      return -1;
    }
    BenchmarkLookup(dic, strs);
    BenchmarkAccess(dic, ids);
  }

  return 0;
}
