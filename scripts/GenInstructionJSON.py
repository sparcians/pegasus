#!/usr/bin/env python3
"""Helper script for generating instruction uarch JSONs.
"""

from enum import Enum
import json
import math
import sys
import os
import argparse

from insts.RVI_INST import RV32I_INST
from insts.RVI_INST import RV64I_INST
from insts.RVM_INST import RV32M_INST
from insts.RVM_INST import RV64M_INST
from insts.RVA_INST import RV32A_INST
from insts.RVA_INST import RV64A_INST
from insts.RVF_INST import RV32F_INST
from insts.RVF_INST import RV64F_INST
from insts.RVD_INST import RV32D_INST
from insts.RVD_INST import RV64D_INST

from insts.RVZFA_INST import RV32ZFA_INST
from insts.RVZFA_INST import RV64ZFA_INST
from insts.RVZFH_INST import RV32ZFH_INST
from insts.RVZFH_INST import RV64ZFH_INST

from insts.RVB_INST    import RV32ZBA_INST
from insts.RVB_INST    import RV64ZBA_INST
from insts.RVB_INST    import RV32ZBB_INST
from insts.RVB_INST    import RV64ZBB_INST
from insts.RVB_INST    import RV32ZBC_INST
from insts.RVB_INST    import RV64ZBC_INST
from insts.RVB_INST    import RV32ZBS_INST
from insts.RVB_INST    import RV64ZBS_INST
from insts.RVZBKB_INST import RV32ZBKB_INST
from insts.RVZBKB_INST import RV64ZBKB_INST

from insts.RVZICSR_INST     import RV32ZICSR_INST
from insts.RVZICSR_INST     import RV64ZICSR_INST
from insts.RVZIFENCEI_INST  import RV32ZIFENCEI_INST
from insts.RVZIFENCEI_INST  import RV64ZIFENCEI_INST

from insts.RVZICBOP_INST import RV32ZICBOP_INST
from insts.RVZICBOP_INST import RV64ZICBOP_INST
from insts.RVZICBOM_INST import RV32ZICBOM_INST
from insts.RVZICBOM_INST import RV64ZICBOM_INST
from insts.RVZICBOZ_INST import RV32ZICBOZ_INST
from insts.RVZICBOZ_INST import RV64ZICBOZ_INST

from insts.RVZIHINTNTL_INST import RV32ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTNTL_INST import RV64ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTPAUSE_INST import RV32ZIHINTPAUSE_INST     # Zihintpause
from insts.RVZIHINTPAUSE_INST import RV64ZIHINTPAUSE_INST     # Zihintpause

from insts.RVZICOND_INST import RV32ZICOND_INST
from insts.RVZICOND_INST import RV64ZICOND_INST
from insts.RVZCMP_INST import RV32ZCMP_INST
from insts.RVZCMP_INST import RV64ZCMP_INST
from insts.RVZCMT_INST import RV32ZCMT_INST
from insts.RVZCMT_INST import RV64ZCMT_INST
from insts.RVZABHA_INST import RV32ZABHA_INST
from insts.RVZABHA_INST import RV64ZABHA_INST
from insts.RVZILSD_INST import RV32ZILSD_INST

from insts.RVV_INST import RVZVE32X_INST
from insts.RVV_INST import RVZVE32F_INST
from insts.RVV_INST import RVZVE64D_INST
from insts.RVV_INST import RVZVE64X_INST

# Mavis extensions
from insts.RVI_INST import RVI_MAVIS_EXTS
from insts.RVM_INST import RVM_MAVIS_EXTS
from insts.RVA_INST import RVA_MAVIS_EXTS
from insts.RVF_INST import RVF_MAVIS_EXTS
from insts.RVD_INST import RVD_MAVIS_EXTS
from insts.RVZFH_INST import RVZFH_MAVIS_EXTS

from insts.RVB_INST   import RVB_MAVIS_EXTS
from insts.RVZBKB_INST  import RVZBKB_MAVIS_EXTS

from insts.RVZICSR_INST     import RVZICSR_MAVIS_EXTS
from insts.RVZIFENCEI_INST  import RVZIFENCEI_MAVIS_EXTS

from insts.RVZICBOP_INST import RVZICBOP_MAVIS_EXTS
from insts.RVZICBOM_INST import RVZICBOM_MAVIS_EXTS
from insts.RVZICBOZ_INST import RVZICBOZ_MAVIS_EXTS

from insts.RVZIHINTNTL_INST import RVZIHINTNTL_MAVIS_EXTS
from insts.RVZIHINTPAUSE_INST import RVZIHINTPAUSE_MAVIS_EXTS

from insts.RVZICOND_INST import RVZICOND_MAVIS_EXTS
from insts.RVZCMP_INST import RVZCMP_MAVIS_EXTS
from insts.RVZCMT_INST import RVZCMT_MAVIS_EXTS
from insts.RVZABHA_INST import RVZABHA_MAVIS_EXTS
from insts.RVZILSD_INST import RV32ZILSD_MAVIS_EXTS # RV32 only
from insts.RVZFA_INST import RVZFA_MAVIS_EXTS

from insts.RVV_INST import RVV_MAVIS_EXTS

class InstJSONGenerator():
    """Generates instruction definition JSON files.

    Args:
        xlen (str): RISC-V XLEN string (e.g. "32" or "64")
        extensions (list): List of supported extensions (e.g. "i", "v", "zicsr")
    """
    def __init__(self, xlen, pegasus_uarch_jsons):
        self.xlen = xlen
        self.pegasus_uarch_jsons = pegasus_uarch_jsons
        self.isa_map = {}

        for ext in self.pegasus_uarch_jsons:
            xlen_str = "" if "zve" in ext else str(self.xlen)
            global_str = "RV" + xlen_str + ext.upper() + "_INST"
            self.isa_map[ext] = globals()[global_str]
            for inst in self.isa_map[ext]:
                inst['xlen'] = self.xlen

            # Sort alphabetically
            self.isa_map[ext] = sorted(self.isa_map[ext], key=lambda x: x['mnemonic'])

    def write_jsons(self, arch_name):
        """Write register definitions to a JSON file in the given directory"""
        xlen_str = "rv"+str(self.xlen)
        gen_path = os.path.join(arch_name, xlen_str, 'gen')

        if not os.path.isdir(gen_path):
            os.makedirs(gen_path)

        for ext in self.pegasus_uarch_jsons:
            filename = os.path.join(gen_path, "pegasus_uarch_" + xlen_str + ext.lower() + ".json")
            with open(filename,"w") as fh:
                json.dump(self.isa_map[ext], fh, indent=4)

def get_exts():
    ''' Generates a string with all of the available extensions imported.
        The format of the returned string is xlen_extension1_extension2_...'''
    rv64_exts = list()
    rv32_exts = list()

    for var in globals().keys():
        if "MAVIS_EXTS" in var:
            if "64" in var:
                rv64_exts.extend(globals()[var])
            elif "32" in var:
                rv32_exts.extend(globals()[var])
            else:
                rv64_exts.extend(globals()[var])
                rv32_exts.extend(globals()[var])
        else:
            continue

    return (rv64_exts, rv32_exts)

def get_pegasus_uarch_jsons():
    ''' Generates a string with all of the available extensions imported.
        The format of the returned string is xlen_extension1_extension2_...'''
    rv64_jsons = list()
    rv32_jsons = list()

    for var in globals().keys():
        if "RV" in var and "MAVIS" not in var:
            var = var.replace("_INST", "")
            var = var.lower()
            if "rv64" in var:
                var = var.replace("rv64", "")
                rv64_jsons.append(var)
            elif "rv32" in var:
                var = var.replace("rv32", "")
                rv32_jsons.append(var)
            else:
                var = var.replace("rv", "")
                rv64_jsons.append(var)
                rv32_jsons.append(var)
        else:
            continue

    return (rv64_jsons, rv32_jsons)

def gen_supported_isa_header(arch_root, supported_rv64_exts, supported_rv32_exts, pegasus_uarch_rv64_jsons, pegasus_uarch_rv32_jsons):
    ''' Writes header file with ISA defines '''
    default_isa = "imafdcbv_zicsr_zifencei"

    gen_path = 'gen'
    if not os.path.isdir(gen_path):
        os.makedirs(gen_path)

    with open( os.path.join(gen_path, 'supportedISA.hpp'), 'w' ) as fh:
        fh.write( '#pragma once\n\n' )
        cmdline = ' '.join(sys.argv)
        fh.write( f'// This file is autogenerated using: {cmdline}\n\n' )
        fh.write( f'#define DEFAULT_ISA_STR "{default_isa}"\n' )
        fh.write('#define SUPPORTED_RV64_EXTS {\\\n')
        for idx, ext in enumerate(supported_rv64_exts):

            if idx == (len(supported_rv64_exts) - 1):
                fh.write( f'    \"{ext}\" }}\n')
            else:
                fh.write( f'    \"{ext}\", \\\n')
        fh.write('#define SUPPORTED_RV32_EXTS {\\\n')
        for idx, ext in enumerate(supported_rv32_exts):
            # Last extension in the list
            if idx == (len(supported_rv32_exts) - 1):
                fh.write( f'    \"{ext}\" }}\n')
            else:
                fh.write( f'    \"{ext}\", \\\n')

def main():
    SUPPORTED_XLEN = ['rv32', 'rv64']

    # Parse arguments
    parser = argparse.ArgumentParser(description="RISC-V Helper script for generating instruction uarch JSONs")
    parser.add_argument("xlen", choices=SUPPORTED_XLEN, metavar='xlen', nargs="?",
                        help=f"XLEN ({', '.join(SUPPORTED_XLEN)})")
    parser.add_argument("--gen-supported-isa-header", action="store_true",
                        help="Only generates the supported/default ISA header files")
    args = parser.parse_args()

    pegasus_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    arch_root = os.path.join(pegasus_root, 'arch')

    os.chdir(arch_root)

    rv64_exts, rv32_exts = get_exts()
    pegasus_uarch_rv64_jsons, pegasus_uarch_rv32_jsons = get_pegasus_uarch_jsons()

    if args.gen_supported_isa_header:
        gen_supported_isa_header(arch_root, rv64_exts, rv32_exts, pegasus_uarch_rv64_jsons, pegasus_uarch_rv32_jsons)
        return

    # RVA23
    # FIXME: At some point, we will read in the Mavis profile JSONs to get the
    # list of supported extensions. For now, I've hardcoded it here.
    RVA23_RV64_SUPPORTED_EXTENSIONS = ['i', 'c', 'zca', 'zcb', 'm', 'zmmul',
                                       'a', 'zalrsc', 'zaamo', 'f', 'zcf',
                                       'd', 'zcd', 'zfh', 'zfhmin', 'zfhmind',
                                       'b', 'zba', 'zbb', 'zbc', 'zbs', 'zbkb',
                                       'zicsr', 'zifencei', 'zicbop', 'zicbom',
                                       'zicboz', 'zihintntl', 'zihintpause',
                                       'zicond', 'zcmp', 'zcmt', 'zabha', 'zfa',
                                       'v', 'zve32x', 'zve32f', 'zve64f', 'zve64d',
                                       'zve64x']
    RVA23_RV32_SUPPORTED_EXTENSIONS = ['i', 'c', 'zca', 'zcb', 'm', 'zmmul',
                                       'a', 'zalrsc', 'zaamo', 'f', 'zcf',
                                       'd', 'zcd', 'zfh', 'zfhmin', 'zfhmind',
                                       'b', 'zba', 'zbb', 'zbc', 'zbs', 'zbkb',
                                       'zicsr', 'zifencei', 'zicbop', 'zicbom',
                                       'zicboz', 'zihintntl', 'zihintpause',
                                       'zicond', 'zcmp', 'zcmt', 'zabha', 'zilsd', 'zfa',
                                       'v', 'zve32x', 'zve32f', 'zve64f', 'zve64d',
                                       'zve64x']
    rva23_r64_exts = [ext for ext in rv64_exts if ext in RVA23_RV64_SUPPORTED_EXTENSIONS]
    rva23_r32_exts = [ext for ext in rv32_exts if ext in RVA23_RV32_SUPPORTED_EXTENSIONS]
    rva23_pegasus_uarch_rv64_jsons = [ext for ext in pegasus_uarch_rv64_jsons if ext in RVA23_RV64_SUPPORTED_EXTENSIONS]
    rva23_pegasus_uarch_rv32_jsons = [ext for ext in pegasus_uarch_rv32_jsons if ext in RVA23_RV32_SUPPORTED_EXTENSIONS]

    # Instruction uarch jsons
    if (args.xlen == "rv64"):
        inst_handler_gen = InstJSONGenerator("64", rva23_pegasus_uarch_rv64_jsons)
        inst_handler_gen.write_jsons("rva23")
    else:
        inst_handler_gen = InstJSONGenerator("32", rva23_pegasus_uarch_rv32_jsons)
        inst_handler_gen.write_jsons("rva23")


if __name__ == "__main__":
    main()
