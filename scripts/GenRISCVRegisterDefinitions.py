#!/usr/bin/env python3
"""Helper script for regenerating register definition JSON files.
"""

from enum import Enum
import os, shutil
import argparse

from GenRegisterJSON import GenRegisterJSON
from GenRegisterJSON import RegisterGroup

from GenCSRHeaders import gen_csr_num_header
from GenCSRHeaders import gen_csr_helpers_header
from GenCSRHeaders import gen_csr_bitmask_header
from GenCSRHeaders import gen_csr_field_idxs_header

def setup_arch_dirs(arch_root, arch_name):
    arch_name_root = os.path.join(arch_root, arch_name)
    if not os.path.exists(arch_name_root):
        os.makedirs(arch_name_root)

    os.chdir(arch_name_root)

    rv64_root = os.path.join(arch_name_root, 'rv64', 'gen')
    if not os.path.exists(rv64_root):
        os.makedirs(rv64_root)

    rv32_root = os.path.join(arch_name_root, 'rv32', 'gen')
    if not os.path.exists(rv32_root):
        os.makedirs(rv32_root)

    return rv64_root, rv32_root


def gen_rva23_reg_defs(rv64_root, rv32_root):
    # Generate rv64g int, fp and CSR registers
    os.chdir(rv64_root)
    rv64_regs = {}
    RV64_XLEN = 8
    SUPPORTED_VLENS = [128, 256, 512, 1024, 2048]
    rv64_regs["int"] = GenRegisterJSON(RegisterGroup.INT, 32, RV64_XLEN)
    rv64_regs["fp"]  = GenRegisterJSON(RegisterGroup.FP,  32, RV64_XLEN)
    for vlen in SUPPORTED_VLENS:
        name = "vec" + str(vlen)
        rv64_regs[name] = GenRegisterJSON(RegisterGroup.VEC, 32, int(vlen/8))
    rv64_regs["csr"] = GenRegisterJSON(RegisterGroup.CSR, 0,  RV64_XLEN)

    # Add register for the PC
    num = 32
    rv64_regs["int"].add_custom_register("pc", num, "Program counter", 8, [], {}, True)

    # Add rv64_regs for atomic load-reservation and store-conditional instructions
    num += 1
    rv64_regs["int"].add_custom_register("resv_addr", num, "Load reservation address", 8, [], {}, True)
    num += 1
    rv64_regs["int"].add_custom_register("resv_valid", num, "Load reservation valid", 8, [], {}, True)

    for name, regs in rv64_regs.items():
        contexts = regs.get_reg_ctx()

        if len(contexts) > 1:
            for ctx in contexts:
                filename = f'reg_{name}_{ctx.lower()}.json'
                regs.write_json(filename, ctx)
        else:
            filename = f'reg_{name}.json'
            regs.write_json(filename)

    # Generate rv32g int, fp and CSR registers
    os.chdir(rv32_root)
    rv32_regs = {}
    RV32_XLEN = 4
    rv32_regs["int"] = GenRegisterJSON(RegisterGroup.INT, 32, RV32_XLEN)
    rv32_regs["fp"] = GenRegisterJSON(RegisterGroup.FP,  32, RV32_XLEN)
    # Vector rv32_regs are the same for RV32
    for vlen in SUPPORTED_VLENS:
        name = "vec" + str(vlen)
        rv32_regs[name] = rv64_regs[name]
    rv32_regs["csr"] = GenRegisterJSON(RegisterGroup.CSR, 0,  RV32_XLEN)

    # Add register for the PC
    num = 32
    rv32_regs["int"].add_custom_register("pc", num, "Program counter", 4, [], {}, True)

    # Add rv32_regs for atomic load-reservation and store-conditional instructions
    num += 1
    rv32_regs["int"].add_custom_register("resv_addr", num, "Load reservation address", 4, [], {}, True)
    num += 1
    rv32_regs["int"].add_custom_register("resv_valid", num, "Load reservation valid", 4, [], {}, True)

    for name, regs in rv32_regs.items():
        contexts = regs.get_reg_ctx()

        if len(contexts) > 1:
            for ctx in contexts:
                filename = f'reg_{name}_{ctx.lower()}.json'
                regs.write_json(filename, ctx)
        else:
            filename = f'reg_{name}.json'
            regs.write_json(filename)

    return rv64_regs, rv32_regs


def main():
    parser = argparse.ArgumentParser(description="RISC-V Helper script for generating register JSONs")
    parser.add_argument("--gen-csr-headers", action="store_true",
                        help="Only generates the CSR header files")
    args = parser.parse_args()

    pegasus_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    arch_root = os.path.join(pegasus_root, 'arch')
    os.chdir(arch_root)

    # RVA23
    rv64_root, rv32_root = setup_arch_dirs(arch_root, "rva23")
    rv64_regs, rv32_regs = gen_rva23_reg_defs(rv64_root, rv32_root)

    # Generate Pegasus header files
    # FIXME: Currently RVA23 includes everything so its ok to use its register
    # definitions to generate the CSR headers but when multiple architectures
    # are supported we may need to create a super set of all registers
    if args.gen_csr_headers:
        inc_root = os.path.join(pegasus_root, 'include')
        if not os.path.exists(inc_root):
            os.makedirs(inc_root)
        if not os.path.exists(os.path.join(inc_root, 'gen')):
            os.makedirs(os.path.join(inc_root, 'gen'))

        os.chdir(inc_root)
        csr_num_hpp = gen_csr_num_header()
        csr_helpers_hpp = gen_csr_helpers_header()
        csr_field_idxs32_hpp = gen_csr_field_idxs_header(rv32_regs, 4)
        csr_field_idxs64_hpp = gen_csr_field_idxs_header(rv64_regs, 8)
        csr_bitmasks32_hpp = gen_csr_bitmask_header(4)
        csr_bitmasks64_hpp = gen_csr_bitmask_header(8)


if __name__ == "__main__":
    main()
