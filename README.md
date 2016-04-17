# cda-tries

This library provides a C++ implementation of compressed double-array tries for static string dictionaries.

## How to compile the library

We can compile it by using a CMake build system as follows:

	$ cmake . -DCMAKE_BUILD_TYPE=Release
	$ make


## How to use the library

The library provides a simple command-line script for building and testing dictionaries.

```
$ ./cda-tries 
cda-tries <mode> <type> <file> <index>
	<mode>  Running mode
		b: Build mode, t: Test mode
	<type>  Representation type of BASE and CHECK
		p: Plain, d: DACs, f: Fast DACs
	<file>  File path of strings
	<index> Index path of dictionary
```

If you want to build a dictionary `index.dacs` from a file `sorted-strings` by using DAC representation, please enter the following command:

```
$ ./cda-tries b d sorted-strings index.dacs
```

Note that `sorted-strings` must be lexicographically sorted.
In addition, the strings must be ended with the `'\n'` ASCII char and must not include the `'\0'` ASCII char.


If you want to test the dictionary `index.dacs` by using a file `test-strings`, please enter the following command:

```
$ ./cda-tries t d test-strings index.dacs
```

It outputs the status of `index.dacs` and tests `lookup` for strings in `test-strings` and `access` for the IDs corresponding to the strings.
Note that `test-strings` must not be lexicographically sorted.
