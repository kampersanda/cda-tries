# cda-tries/previous-tries

This library also implements compressed double-array trie dictionaries supporting only Lookup, described in the articles.

* S. Yata, M. Oono, K. Morita, M. Fuketa, T. Sumitomo, and J. Aoe, "A compact static double-array keeping character codes", _Information Processing & Management_, 43(1): 237–247, 2007. [[doi](http://dx.doi.org/10.1016/j.ipm.2006.04.004)]
* S. Kanda, M. Fuketa, K. Morita, and J. Aoe, "A compression method of double-array structures using linear functions", _Knowledge and Information Systems_, 48(1): 55–80, 2016. [[doi](http://dx.doi.org/10.1007/s10115-015-0873-0)]

## How to benchmark the dictionaries

The library provides a simple command-line script for building and testing dictionaries.

```
$ ./previous-tries/BenchmarkPrev 
Benchmark <mode> <type> <str_path> <dic_path>
  <mode> Running mode
         1: Build, 2: Benchmark
  <type> CDA type
         1: CDA, 2: DALF
  <str_path> File path of strings
  <dic_path> File path of dictionary
```

The usage is the same as `./Benchmark`.