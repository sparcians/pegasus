# Atlas
RISC-V Functional Model

## Build

Download and build the [Sparta](https://github.com/sparcians/map) framework. Follow the directions on the [Sparta README](https://github.com/sparcians/map#readme) to build _and install_ Sparta.

```
conda activate sparta
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Regress
```
make regress
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

## IDE Support

To produce `compile_commands.json`, add this parameter to the CMake command.
```
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```
