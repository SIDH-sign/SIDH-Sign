# SIDH-sign

## Description
---

We analyze and implement the SIDH PoK-based construction from De Feo, Dobson, Galbraith, and Zobernigl.
We improve the sidh-sign built-in functions to allow an efficient constant-time implementation.
After that, we combine it with Fiat-Shamir transform to get an SIDH PoK-based signature scheme that we short label as SIDH-sign.

### Primes supported

We support SIDH primes (377, 434, 503, 546, 610, 697, 751).


## Compilation 

Compilation for Linux and MacOS using CMAKE

### cmake configuration

First configure the build directory with
```bash
cmake -DCMAKE_BUILD_TYPE=<BUILD_TYPE> -B build
```
where **<BUILD_TYPE>** is either **Release** or **Debug**

Optionally, add personal preference to CMAKE such as
```bash
-DCMAKE_MAKE_PROGRAM=path/to/ninja -G Ninja
```

### Optional optimizations

Add `-DOPT=ASM` to include optimized assembly code for the prime field arithmetic
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DOPT=ASM -B build
```

### make library targets

Jump into buiild folder and build all target libraries for all supported primes:
```bash
cd build
make -j8
```

To build target libraries individually, simply run
```bash
make sidh-sign-p<PRIME>
```
where **<PRIME>** is one of the supported primes.


### make test targets

Building all targets from last step will create all test targets as well.
To compile specific test target, simply run
```bash
make tests-sidh-sign-p<PRIME>
```
where **<PRIME>** is one of the supported primes.

## Tests
---

Ctest is supported and can be used to call tests using regex. For example:
```bash
cd build
ctest
```
will run all tests, or preferably, use the make target for faster parallel run
```bash
make test-parallel
```


```bash
ctest -V -R test
```
will run all tests in verbose mode without benchmark tests.


```bash
ctest -V -R bench
```
will run only the benchmark tests in verbose mode.

Individual test suites can be run similarly as
```bash
ctest -V -R test-prime_field-sidh-sign-p377
```
or a single test case by calling directly the test app as

```bash
cd build/tests
./tests-sidh-sign-p377 unit/prime_field/addition_subtraction
```

Finally, to run benchmark with more iterations, you can modify the number of times each test is evaluated with the following.
```bash
./tests-sidh-sign-p377 --iterations 10000 bench/
```


### Benchmark remarks

We use munit to run tests and benchmarks. To properly measure the scheme, we 
recommend disabling munit forking. Additionally, for SIDH, POK and signature benchmarks
the output is sent to stderr. The following command can be used to call the test
library and run benchmarks, printing the output to a file. Replace PRIME as one of
the supported primes described above.

```bash
    tests-sidh-sign-<PRIME> bench --show-stderr --no-fork --param iterations 10 1> /dev/null 2> result-<PRIME>
￥
```
