#!/bin/bash

#
# This script is copied the install location for standalone execution
# of Pegasus.
#

#
# This script expects the following directory structure:
#
# $INSTALL_DIR/
#     bin/
#        run_pegasus.sh <-- Users run this
#     share/
#        pegasus/
#           cfg/
#              arch/       <-- Pegasus arch files
#              mavis_json/ <-- Mavis json files used by Pegasus
#           exe/
#              pegasus     <-- The binary

PEGASUS_BIN_PATH=$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)
PEGASUS_CFGS="$PEGASUS_BIN_PATH/../share/pegasus/cfg"
"$PEGASUS_BIN_PATH/../share/pegasus/exe/pegasus" \
    --arch-search-dir               $PEGASUS_CFGS/arch \
    -p top.*.params.isa_file_path   $PEGASUS_CFGS/mavis_json \
    -p top.*.params.uarch_file_path $PEGASUS_CFGS/arch \
    "${@}"
