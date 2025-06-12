#!/usr/bin/env bash

set -x

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "PWD:" `pwd`

CXX_COMPILER=${COMPILER/clang/clang++}
CXX_COMPILER=${CXX_COMPILER/gcc/g++}

cd ${GITHUB_WORKSPACE}

# TODO: Build tests from source
# Build RISC-V arch tests
#cd riscv-arch-tests
#autoconf 
#./configure --prefix=???
#make
#cd ../

# Extract RISC-V arch tests
tar -xf riscv-tests.tar.gz
RISCV_ARCH_TESTS=$GITHUB_WORKSPACE/riscv-tests/isa

# Extract Tenstorrent tests
cd tenstorrent-tests
tar -xf release.tar
TENSTORRENT_TESTS=$GITHUB_WORKSPACE/tenstorrent-tests/release/bare_metal/user
cd ../

# Run Full Regression
cd $ATLAS_BUILD_TYPE
cd sim
python $GITHUB_WORKSPACE/scripts/RunArchTests.py --riscv-arch $RISCV_ARCH_TESTS --tenstorrent $TENSTORRENT_TESTS 
