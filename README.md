# cda-tries

This library provides a C++ implementation of compressed double-array tries for a static string dictionary, which supports two primitive operations:

* Lookup(q) returns the ID in [0, N) if string q is stored, where N is the number of strings.
* Access(i) returns the string with ID i in [0, N), that is, Access(Lookup(q)) = q if q is stored.

## How to compile the library

We can compile it by using a CMake build system as follows:

```
$ cmake . -DCMAKE_BUILD_TYPE=Release -DENABLE_YCHECK=ON
$ make
```

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

If you want to build a dictionary `dict.dac` from a file `strs.sorted` by using a DAC representation, please enter the following command:

```
$ ./Benchmark 1 2 strs.sorted dict.dac
```

Note that `strs.sorted` must be lexicographically sorted.
In addition, the strings must be ended with the `'\n'` ASCII char and must not include the `'\0'` ASCII char.


If you want to test the dictionary `dict.dac` by using a file `strs.test`, please enter the following command:

```
$ ./Benchmark 2 2 strs.test dict.dac
```

It outputs the status of `dict.dac` and tests Lookup for strings in `strs.test` and Access for the IDs corresponding to the strings.
Note that `strs.test` must not be lexicographically sorted.
