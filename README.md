# cda-tries

This C++ library implements compressed double-array tries for a static string dictionary, described in the article.

* S. Kanda, K. Morita, and M. Fuketa, "[Compressed double-array tries for string dictionaries supporting fast lookup](http://dx.doi.org/10.1007/s10115-016-0999-8)", _Knowledge and Information Systems_, online first. [[preprint](https://sites.google.com/site/shnskknd/kais2016.pdf)]

In addition CDA and DALF, which are previous compressed double-array tries, are included at [previous-tries](https://github.com/kamp78/cda-tries/tree/master/previous-tries).

## Notation

I will not update the library.
For XCDA proposed in the article, I recommend use of [xcdat library](https://github.com/kamp78/xcdat) that is an enhanced version.

## How to compile the library

We can compile it by using a CMake build system as follows:

```
$ cmake . -DCMAKE_BUILD_TYPE=Release -DENABLE_YCHECK=ON
$ make
```

Please set the compile option `ENABLE_YCHECK` to `OFF` if you use a conventional construction algorithm.

## How to benchmark the dictionaries

The library provides a simple command-line script for building and testing dictionaries.

```
$ ./Benchmark 
Benchmark <mode> <type> <str_path> <dic_path>
  <mode> Running mode
         1: Build, 2: Benchmark
  <type> Representation type of BASE and CHECK
         1: Plain, 2: DACs, 3: Fast DACs
  <str_path> File path of strings
  <dic_path> File path of dictionary
```

If you build a dictionary `dict.dac` from a string file `strs.sorted` by using a DAC representation, please enter the following command:

```
$ ./Benchmark 1 2 strs.sorted dict.dac
```

Note that `strs.sorted` must be lexicographically sorted and the strings must not include the `'\0'` ASCII char.

If you test the dictionary `dict.dac` by using a string file `strs.test`, please enter the following command:

```
$ ./Benchmark 2 2 strs.test dict.dac
```

It outputs the status of `dict.dac` and tests Lookup for strings in `strs.test` and Access for the IDs corresponding to the strings.
Note that `strs.test` must not be lexicographically sorted.
