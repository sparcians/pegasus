#!/usr/bin/env python3
"""Helper script for generating instruction uarch JSONs.
"""

import os
import argparse

from GenInstructionJSON import get_supported_exts
from GenInstructionJSON import get_pegasus_uarch_jsons
from GenInstructionJSON import gen_supported_isa_header
from GenInstructionJSON import write_pegasus_uarch_jsons

def main():
    SUPPORTED_XLEN = ['rv32', 'rv64']

    # Parse arguments
    parser = argparse.ArgumentParser(description="RISC-V Helper script for generating instruction uarch JSONs")
    parser.add_argument("--gen-supported-isa-header", action="store_true",
                        help="Only generates the supported/default ISA header files")
    args = parser.parse_args()

    pegasus_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    arch_root = os.path.join(pegasus_root, 'arch')

    os.chdir(arch_root)

    RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS = get_supported_exts()
    pegasus_uarch_rv64_jsons, pegasus_uarch_rv32_jsons = get_pegasus_uarch_jsons()

    if args.gen_supported_isa_header:
        gen_supported_isa_header(arch_root, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS, pegasus_uarch_rv64_jsons, pegasus_uarch_rv32_jsons)
        return

    MAVIS_RISCV_JSON = "../mavis/json/riscv_isa_spec.json"

    # RVA23
    write_pegasus_uarch_jsons("rva23", "rva23s64", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)
    # RVB23
    write_pegasus_uarch_jsons("rvb23", "rvb23s64", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)
    # RVM23
    write_pegasus_uarch_jsons("rvm23", "rvm23u32", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)

    # default
    if not os.path.isdir("default"):
        os.symlink("rva23", "default")

if __name__ == "__main__":
    main()
