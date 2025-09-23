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

from insts.RVZBA_INST import RV32ZBA_INST
from insts.RVZBA_INST import RV64ZBA_INST
from insts.RVZBB_INST import RV32ZBB_INST
from insts.RVZBB_INST import RV64ZBB_INST
from insts.RVZBC_INST import RV32ZBC_INST
from insts.RVZBC_INST import RV64ZBC_INST
from insts.RVZBS_INST import RV32ZBS_INST
from insts.RVZBS_INST import RV64ZBS_INST

from insts.RVZICSR_INST import RV32ZICSR_INST
from insts.RVZICSR_INST import RV64ZICSR_INST
from insts.RVZIFENCEI_INST import RV32ZIFENCEI_INST
from insts.RVZIFENCEI_INST import RV64ZIFENCEI_INST

from insts.RVZICBOP_INST import RV32ZICBOP_INST
from insts.RVZICBOP_INST import RV64ZICBOP_INST
from insts.RVZICBOM_INST import RV32ZICBOM_INST
from insts.RVZICBOM_INST import RV64ZICBOM_INST
from insts.RVZICBOZ_INST import RV32ZICBOZ_INST
from insts.RVZICBOZ_INST import RV64ZICBOZ_INST

from insts.RVZICOND_INST import RV32ZICOND_INST
from insts.RVZICOND_INST import RV64ZICOND_INST
from insts.RVZCMP_INST import RV32ZCMP_INST
from insts.RVZCMP_INST import RV64ZCMP_INST
from insts.RVZABHA_INST import RV32ZABHA_INST
from insts.RVZABHA_INST import RV64ZABHA_INST
from insts.RVZILSD_INST import RV32ZILSD_INST
from insts.RVZFA_INST import RV32ZFA_INST
from insts.RVZFA_INST import RV64ZFA_INST

from insts.RVZVE64X_INST import RVZVE64X_INST
from insts.RVZVE64D_INST import RVZVE64D_INST
from insts.RVZVE32X_INST import RVZVE32X_INST
from insts.RVZVE32F_INST import RVZVE32F_INST

from insts.RVZIHINTNTL_INST import RV32ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTNTL_INST import RV64ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTPAUSE_INST import RV32ZIHINTPAUSE_INST     # Zihintpause
from insts.RVZIHINTPAUSE_INST import RV64ZIHINTPAUSE_INST     # Zihintpause

class InstJSONGenerator():
    """Generates instruction definition JSON files.

    Args:
        isa_str (str): RISC-V ISA string (e.g. rv64imafd)
    """
    def __init__(self, isa_str):
        self.isa_str = isa_str

        # Get xlen (32 or 64)
        if "rv32" in isa_str:
            self.xlen = 32
            self.xlen_str = "rv32"
        elif "rv64" in isa_str:
            self.xlen = 64
            self.xlen_str = "rv64"
        else:
            print("Invalid ISA string:", isa_str)
            return

        # Parse ISA string
        self.extensions = []
        isa_str = isa_str.removeprefix("rv32").removeprefix("rv64")
        for ext in isa_str.split('_'):
            if len(ext) == 1 or "Z" == ext[0] or "z" == ext[0]:
                self.extensions.append(ext)
            else:
                for x in ext:
                    if "g" == x:
                        self.extensions.extend(['i', 'm', 'a', 'f', 'd'])
                    elif "b" == x:
                        self.extensions.extend(['zba', 'zbb', 'zbc', 'zbs'])
                    elif "v" == x:
                        self.extensions.extend(['zve64x', 'zve64d', 'zve32x', 'zve32f'])
                    else:
                        self.extensions.append(x)

        self.isa_map = {}
        for ext in self.extensions:
            xlen_str = "" if "zve" in ext else str(self.xlen)
            global_str = "RV" + xlen_str + ext.upper() + "_INST"
            self.isa_map[ext] = globals()[global_str]
            for inst in self.isa_map[ext]:
                inst['xlen'] = self.xlen

            # Sort alphabetically
            self.isa_map[ext] = sorted(self.isa_map[ext], key=lambda x: x['mnemonic'])

    def write_jsons(self):
        """Write register definitions to a JSON file in the given directory"""

        if not os.path.isdir(self.xlen_str):
            os.makedirs(self.xlen_str)

        for ext in self.extensions:
            filename = self.xlen_str + "/pegasus_uarch_" + self.xlen_str + ext.lower() + ".json"
            with open(filename,"w") as fh:
                json.dump(self.isa_map[ext], fh, indent=4)

def get_supported_exts():
    ''' Generates a string with all of the available extensions imported.
        The format of the returned string is xlen_extension1_extension2_...'''
    rv64_exts = list()
    rv32_exts = list()

    for var in globals().keys():
        if "RV" == var[0:2]:
            var = var.replace('_INST','').lower()
            var = var.replace('rv','')
            if "64" == var[0:2]:
                var = var.replace("64",'')
                rv64_exts.append(var)
            elif "32" == var[0:2]:
                var = var.replace("32",'')
                rv32_exts.append(var)
            else:
                rv64_exts.append(var)
                rv32_exts.append(var)
        else:
            continue

    return (rv64_exts, rv32_exts)

def gen_supported_isa_header(xlen, supported_rv64_exts, supported_rv32_exts):
    ''' Writes header file with ISA defines '''
    default_isa = "imafdcbv_zicsr_zifencei"

    with open( 'supportedISA.hpp', 'w' ) as fh:
        fh.write( '#pragma once\n\n' )
        cmdline = ' '.join(sys.argv)
        fh.write( f'// This file is autogenerated using: {cmdline}\n\n' )
        fh.write('#define SUPPORTED_RV64_EXTS    { ')
        for idx, ext in enumerate(supported_rv64_exts):
            # Last extension in the list
            if idx == (len(supported_rv64_exts) - 1):
                fh.write( f'\"{ext}\", \"c\" }}\n')
            else:
                fh.write( f'\"{ext}\", ')
        fh.write('#define SUPPORTED_RV32_EXTS    { ')
        for idx, ext in enumerate(supported_rv32_exts):
            # Last extension in the list
            if idx == (len(supported_rv32_exts) - 1):
                fh.write( f'\"{ext}\", \"c\" }}\n')
            else:
                fh.write( f'\"{ext}\", ')
        fh.write( f'#define DEFAULT_ISA_STR    "{default_isa}"\n' )

def main():
    SUPPORTED_XLEN = ['rv32', 'rv64']

    # Parse arguments
    parser = argparse.ArgumentParser(description="RISC-V Helper script for generating instruction uarch JSONs")
    
    parser.add_argument("xlen", choices=SUPPORTED_XLEN, metavar='xlen',
                        help=f"XLEN ({', '.join(SUPPORTED_XLEN)})")
    parser.add_argument("--gen-supported-isa-header", action="store_true",
                        help="Only generates the supported/default ISA header files")
    args = parser.parse_args()

    pegasus_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    arch_root = os.path.join(pegasus_root, 'arch')

    os.chdir(arch_root)

    supported_rv64_exts, supported_rv32_exts = get_supported_exts()

    if args.gen_supported_isa_header:
        gen_supported_isa_header(args.xlen, supported_rv64_exts, supported_rv32_exts)
        return

    isa_string = "_".join(supported_rv64_exts) if args.xlen == "64" else "_".join(supported_rv32_exts)
    inst_handler_gen = InstJSONGenerator(args.xlen + isa_string)

    # Instruction uarch jsons
    inst_handler_gen.write_jsons()


if __name__ == "__main__":
    main()
