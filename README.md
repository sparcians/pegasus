# Atlas
RISC-V Functional Model

## Clone
```
git clone --recursive git@github.com:sparcians/atlas.git
```

## Build and Regress

Download and build the [Sparta](https://github.com/sparcians/map) framework. Follow the directions on the [Sparta README](https://github.com/sparcians/map#readme) to build _and install_ Sparta.

```
conda activate sparta
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make regress
```

Alternatively, you can build Atlas without Conda by providing a path to your Sparta repo and Atlas will build the Sparta library for you. Since Sparta already has a make target named `regress`, the Atlas regression make target is renamed to `atlas_regress`.
```
mkdir release
cd release
cmake .. -CMAKE_BUILD_TYPE=Release -DSPARTA_SEARCH_DIR=<full path to map/sparta>
make
make atlas_regress
```

## Validate

Atlas provides a helper script for running the RISC-V architecture unit tests from [riscv-software-src/riscv-tests](https://github.com/riscv-software-src/riscv-tests) Follow the directions on the [riscv-software-src/riscv-tests README](https://github.com/riscv-software-src/riscv-tests#readme) to build the tests.

From the `sim` subdirectory of your build directory, run the following Python script to run the RV64 tests:
```
../../scripts/RunRiscVArchTest.py rv64 $RISCV_TESTS_PATH/isa/
```

## Debug

To build Atlas for debugging, Debug and FastDebug build types are available.
```
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=FastDebug
```
```
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Python IDE
See [Python IDE for Atlas](IDE/README.md)

