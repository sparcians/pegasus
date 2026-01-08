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
#         pegasus           <-- The binary
#         run_pegasus.sh    <-- Users run this
#     include/
#         pegasus/
#             mavis/
#                 arch/     <-- Pegasus uarch files
#                 mavis/
#                     json/ <-- Mavis json files used by Pegasus

PEGASUS_BIN_PATH=$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)
PEGASUS_INC="$PEGASUS_BIN_PATH/../include/pegasus"
"$PEGASUS_BIN_PATH/pegasus" \
    --arch-search-dir               $PEGASUS_INC/arch \
    -p top.*.params.isa_file_path   $PEGASUS_INC/mavis/json \
    -p top.*.params.uarch_file_path $PEGASUS_INC/arch \
    "${@}"
