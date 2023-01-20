# competitive-programming
Personal competitive programming library. Slowly migrating from my locally stored code.

Use at your own risk for competitive programming purposes. Bug reports and feature suggestions are welcomed.
**Never ever use this in production in any manner.**

## Prerequisites
* x86_64, G++ and C++17 support are **required**.
  * This library is NOT portable as it is only for use on major online judges.
    Selecting the wrong C++ standard and/or compiler version on online judges will result in compile error.
  * Due to the widespread use of bit manipulation, all code by default requires architectures that support
    [x86 bit manipulation instruction sets up to BMI2](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set#BMI2_(Bit_Manipulation_Instruction_Set_2)).
    If your platform already enables architectures better than BMI2 by command line (for example, `-march=native`),
    or has old CPUs without BMI2, you can disable forced BMI2 target for certain bit manipulation functions
    by `#define _CPLIB_NO_FORCE_BMI2_` before `#include`-ing anything from this library.
* [CMake](https://cmake.org/) for building and running tests.
* [Doxygen](https://www.doxygen.nl/) for building documentation.

## Usage
Most if not all online judges only allow submission of a single file.
`oj-bundle` from [verification-helper](https://github.com/online-judge-tools/verification-helper)
is the recommended tool for bundling all included files into a single file.

To bundle `your_code.cpp` that uses this library by `#include "cplib/..."`, run:
```sh
oj-bundle -I /path/to/competitive-programming/src your_code.cpp > your_code.bundle.cpp
```

Also see files in `submissions/` for examples.

## Testing
This project uses [Catch2](https://github.com/catchorg/Catch2/tree/v2.x) as the unit test framework, which is already
included in this repository as a git submodule. 

Unit tests are located in `test/`. To build and run all unit tests, run:
```sh
cmake -B build
cd build
make
# You can also directly run the executable ./run_tests
make test
```

Integration tests are located in `submissions/`. They are solutions to actual problems on various online judges.
Use `oj-verify` from [verification-helper](https://github.com/online-judge-tools/verification-helper) to run them
locally or submit them to online judges. For example:
```sh
# File name must be *.test.cpp for oj-verify to recognize
oj-bundle -I /path/to/competitive-programming/src example.cpp > example.test.cpp
oj-verify run
```

## Documentation
View documentation at https://cai-lw.github.io/competitive-programming/.

Run `scripts/build_and_publish_docs.sh` to build documentation and publish it to GitHub Pages, or run `doxygen` to
generate it locally to `docs/`.