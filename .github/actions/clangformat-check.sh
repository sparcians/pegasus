#!/usr/bin/env bash

set -x

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "PWD:" `pwd`

cd ${GITHUB_WORKSPACE}
cd $PEGASUS_BUILD_TYPE
make clangformat-check
CHECK_PEGASUS=$?
if [ ${CHECK_PEGASUS} -ne 0 ]; then
    echo "ERROR: clangformat check of Pegasus FAILED!!!"
    exit 1
fi
