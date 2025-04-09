# Atlas
RISC-V Functional Model

## Install Prerequisites

Install the following packages (tested with Ubuntu 24.04):

- (apt-get install cmake) cmake v3.22
- (apt-get install libboost-all-dev) boost 1.74.0
- (apt-get install yaml-cpp-dev) YAML CPP 0.7.0
- (apt-get install rapidjson-dev) RapidJSON CPP 1.1.0
- (apt-get install libsqlite3-dev) SQLite3 3.37.2
- (apt-get install libhdf5-dev) HDF5 1.10.7
- (apt-get install clang) Clang, Version: 14.0.0

Download and build sparta, map_v2:
```
git clone git@github.com:sparcians/map
cd map/sparta
mkdir release
cd release
CC=$COMPILER CXX=$CXX_COMPILER cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/
sudo make -j$(nproc --all) install
```

## Clone Atlas and Build/Regress
```
git clone --recursive git@github.com:sparcians/atlas.git
cd atlas
mkdir release
cd release
cmake .. -DCMAKE_BUILD_TYPE=Release -DSPARTA_SEARCH_DIR=/usr/local
make
make atlas_regress
```
Alternatively, you can build Atlas without an install of Sparta by providing a path
to your Sparta repo and Atlas will build the Sparta library for
you. Since Sparta already has a make target named `regress`, the Atlas
regression make target is renamed to `atlas_regress`.

```
mkdir release
cd release
cmake .. -CMAKE_BUILD_TYPE=Release -DSPARTA_SEARCH_DIR=<full path to map/sparta>
make
make atlas_regress
```

## Validate

Atlas provides a helper script for running the RISC-V architecture
unit tests from
[riscv-software-src/riscv-tests](https://github.com/riscv-software-src/riscv-tests)
Follow the directions on the [riscv-software-src/riscv-tests
README](https://github.com/riscv-software-src/riscv-tests#readme) to
build the tests.

From the `sim` subdirectory of your build directory, run the following Python script to run the RV64 tests:
```
cd atlas/<build>/sim
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
