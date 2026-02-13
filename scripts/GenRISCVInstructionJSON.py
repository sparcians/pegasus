#!/usr/bin/env python3
"""Helper script for generating instruction uarch JSONs.
"""

import os
import argparse

from GenInstructionJSON import get_supported_exts
from GenInstructionJSON import get_pegasus_uarch_jsons
from GenInstructionJSON import gen_supported_isa_header
from GenInstructionJSON import get_profile_supported_exts
from GenInstructionJSON import InstJSONGenerator

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

    # Default (everything included)
    inst_handler_gen = InstJSONGenerator(32, RV32_PEGASUS_SUPPORTED_EXTS)
    inst_handler_gen.write_jsons("default")
    inst_handler_gen = InstJSONGenerator(64, RV64_PEGASUS_SUPPORTED_EXTS)
    inst_handler_gen.write_jsons("default")

    # RVA23
    rva23_supported_exts = get_profile_supported_exts("rva23s64", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)
    inst_handler_gen = InstJSONGenerator(64, rva23_supported_exts)
    inst_handler_gen.write_jsons("rva23")
    # RVB23
    rvb23_supported_exts = get_profile_supported_exts("rvb23s64", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)
    inst_handler_gen = InstJSONGenerator(64, rvb23_supported_exts)
    inst_handler_gen.write_jsons("rvb23")
    # RVM23
    rvm23_supported_exts = get_profile_supported_exts("rvm23u32", MAVIS_RISCV_JSON, RV64_PEGASUS_SUPPORTED_EXTS, RV32_PEGASUS_SUPPORTED_EXTS)
    inst_handler_gen = InstJSONGenerator(32, rvm23_supported_exts)
    inst_handler_gen.write_jsons("rvm23")

if __name__ == "__main__":
    main()
