#!/usr/bin/env bash

set -x

source "/usr/share/miniconda/etc/profile.d/conda.sh"
conda activate riscv_func_model

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "CONDA_PREFIX:" $CONDA_PREFIX
echo "PWD:" `pwd`

CXX_COMPILER=${COMPILER/clang/clang++}

cd ${GITHUB_WORKSPACE}
mkdir $ATLAS_BUILD_TYPE
cd $ATLAS_BUILD_TYPE
CC=$COMPILER CXX=$CXX_COMPILER cmake .. -DCMAKE_BUILD_TYPE=$ATLAS_BUILD_TYPE -DGEN_DEBUG_INFO=OFF
if [ $? -ne 0 ]; then
    echo "ERROR: CMake for atlas failed"
    exit 1
fi
make clangformat-check
BUILD_ATLAS=$?
if [ ${BUILD_ATLAS} -ne 0 ]; then
    echo "ERROR: build/regress of Atlas FAILED!!!"
    echo "$(<regress.log)"
    exit 1
fi
