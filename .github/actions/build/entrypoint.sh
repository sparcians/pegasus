#!/usr/bin/env bash

set -x

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "PWD:" `pwd`

CXX_COMPILER=${COMPILER/clang/clang++}
CXX_COMPILER=${CXX_COMPILER/gcc/g++}

#
# Compile Sparta Infra (always build with release)
#   Have other build types point to release
#
echo "Building Sparta Infra"
cd ${GITHUB_WORKSPACE}/map/sparta
mkdir -p release
cd release
CC=$COMPILER CXX=$CXX_COMPILER  cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/
if [ $? -ne 0 ]; then
    echo "ERROR: CMake for Sparta framework failed"
    exit 1
fi
sudo make -j$(nproc --all) install
BUILD_SPARTA=$?
if [ ${BUILD_SPARTA} -ne 0 ]; then
    echo "ERROR: build sparta FAILED!!!"
    echo "$(<install.log)"
    exit 1
fi
rm install.log

cd ${GITHUB_WORKSPACE}
mkdir $ATLAS_BUILD_TYPE
cd $ATLAS_BUILD_TYPE
cmake .. -DSPARTA_SEARCH_DIR=/usr/local -DCMAKE_C_COMPILER=$COMPILER -DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_BUILD_TYPE=$ATLAS_BUILD_TYPE
if [ $? -ne 0 ]; then
    echo "ERROR: CMake for atlas failed"
    exit 1
fi
make -j$(nproc --all) atlas_regress
BUILD_ATLAS=$?
if [ ${BUILD_ATLAS} -ne 0 ]; then
    echo "ERROR: build/regress of Atlas FAILED!!!"
    exit 1
fi
