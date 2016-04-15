#include <iostream>

#include "DaTrie.hpp"

using namespace std;
using namespace cda_tries;

namespace {

static const size_t RUNS = 10;

void ShowUsage() {
  cout << "cda-tries <mode> <type> <file> <index>" << endl;
  cout << "\t<mode>  Running mode" << endl;
  cout << "\t\tb: Build mode, t: Test mode" << endl;
  cout << "\t<type>  Representation type of BASE and CHECK" << endl;
  cout << "\t\tp: Plain, d: DACs, f: Fast DACs" << endl;
  cout << "\t<file>  File path of strings" << endl;
  cout << "\t<index> Index path of dictionary" << endl;
}

void LoadStrings(istream &is, vector<string> &strs) {
  string line;
  while (getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    strs.push_back(line);
  }
  strs.shrink_to_fit();
}

bool TestCheck(const vector<string> &strs,
               const unique_ptr<DaTrie> &trie, vector<uint> &ids) {
  const size_t numStrs = strs.size();
  cout << "Checking for " << numStrs << " strs..." << endl;

  ids.clear();
  ids.reserve(numStrs);

  string str;
  for (size_t i = 0; i < numStrs; ++i) {
    const uint id = trie->Lookup(strs[i].c_str());
    if (id == DaTrie::NOT_FOUND) {
      cerr << "Lookup error\t" << strs[i].c_str() << endl;
      return false;
    }

    trie->Access(id, str);
    if (str.empty()) {
      cerr << "Access error\t" << id << endl;
      return false;
    }
    ids.push_back(id);
  }

  cout << "No error" << endl;
  return true;
}

void TestLookup(const vector<string> &strs, const unique_ptr<DaTrie> &trie) {
  const size_t numStrs = strs.size();
  cout << "Test of Lookup for " << numStrs << " strs..." << endl;
  cout << "# of runs: " << RUNS << endl;

  StopWatch sw;
  for (size_t i = 0; i < RUNS; ++i) {
    for (size_t j = 0; j < numStrs; ++j) {
      trie->Lookup(strs[j].c_str());
    }
  }

  const double aveTime = sw.Get(sw_e::MICRO) / RUNS;
  cout << "Times..." << endl;
  cout << aveTime / numStrs << " us per str" << endl;
}

void TestAccess(const vector<uint> &ids, const unique_ptr<DaTrie> &trie) {
  const size_t numIds = ids.size();
  cout << "Test of Access for " << numIds << " IDs..." << endl;
  cout << "# of runs: " << RUNS << endl;

  StopWatch sw;
  for (size_t i = 0; i < RUNS; ++i) {
    for (size_t j = 0; j < numIds; ++j) {
      string str;
      trie->Access(ids[j], str);
    }
  }

  const double aveTime = sw.Get(sw_e::MICRO) / RUNS;
  cout << "Times..." << endl;
  cout << aveTime / numIds << " us per ID" << endl;
}

} // namespace

int main(int argc, const char *argv[]) {
  if (argc != 5) {
    ShowUsage();
    return -1;
  }

  const char mode = *argv[1];
  if (mode != 'b' && mode != 't') {
    ShowUsage();
    return -1;
  }

  bc_e type;
  switch (*argv[2]) {
    case 'p':
      type = bc_e::PLAIN;
      break;
    case 'd':
      type = bc_e::DAC;
      break;
    case 'f':
      type = bc_e::FDAC;
      break;
    default:
      ShowUsage();
      return -1;
  }

  string fn(argv[3]);
  string index(argv[4]);

  unique_ptr<DaTrie> trie;
  vector<string> strs;

  {
    ifstream ifs(fn);
    if (!ifs) {
      cerr << "Error: failed to open strings: " << fn << endl;
      return -1;
    }
    LoadStrings(ifs, strs);
  }

  // Build
  if (mode == 'b') {
    StopWatch sw;
    trie.reset(new DaTrie(strs, type));
    cout << "Constr. time: " << sw.Get(sw_e::SEC) << " sec" << endl;

    ofstream ofs(index, ios::binary);
    if (!ofs) {
      cerr << "Index open Error: " << index << endl;
      return -1;
    }
    trie->Save(ofs);
  }

  // Test
  if (mode == 't') {
    ifstream ifs(index, ios::binary);
    if (!ifs) {
      cerr << "Index open Error: " << index << endl;
      return -1;
    }

    trie.reset(new DaTrie(ifs, type));
    trie->Stat(cout);

    vector<uint> ids;
    if (!TestCheck(strs, trie, ids)) {
      return -1;
    }
    TestLookup(strs, trie);
    TestAccess(ids, trie);
  }

  return 0;
}
