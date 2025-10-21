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

from insts.RVZBA_INST   import RV32ZBA_INST
from insts.RVZBA_INST   import RV64ZBA_INST
from insts.RVZBB_INST   import RV32ZBB_INST
from insts.RVZBB_INST   import RV64ZBB_INST
from insts.RVZBC_INST   import RV32ZBC_INST
from insts.RVZBC_INST   import RV64ZBC_INST
from insts.RVZBKB_INST  import RV32ZBKB_INST
from insts.RVZBKB_INST  import RV64ZBKB_INST
from insts.RVZBS_INST   import RV32ZBS_INST
from insts.RVZBS_INST   import RV64ZBS_INST

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

from insts.RVZICOND_INST import RV32ZICOND_INST
from insts.RVZICOND_INST import RV64ZICOND_INST
from insts.RVZCMP_INST import RV32ZCMP_INST
from insts.RVZCMP_INST import RV64ZCMP_INST
from insts.RVZCA_INST import RV32ZCA_INST
from insts.RVZCA_INST import RV64ZCA_INST
from insts.RVZCMT_INST import RV32ZCMT_INST
from insts.RVZCMT_INST import RV64ZCMT_INST
from insts.RVZABHA_INST import RV32ZABHA_INST
from insts.RVZABHA_INST import RV64ZABHA_INST
from insts.RVZILSD_INST import RV32ZILSD_INST
from insts.RVZFA_INST import RV32ZFA_INST
from insts.RVZFA_INST import RV64ZFA_INST

from insts.RVZVE64X_INST import RVZVE64X_INST
from insts.RVZVE64D_INST import RVZVE64D_INST
from insts.RVZVE64F_INST import RVZVE64F_INST
from insts.RVZVE32X_INST import RVZVE32X_INST
from insts.RVZVE32F_INST import RVZVE32F_INST

from insts.RVZIHINTNTL_INST import RV32ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTNTL_INST import RV64ZIHINTNTL_INST         # Zihintntl
from insts.RVZIHINTPAUSE_INST import RV32ZIHINTPAUSE_INST     # Zihintpause
from insts.RVZIHINTPAUSE_INST import RV64ZIHINTPAUSE_INST     # Zihintpause

class InstJSONGenerator():
    """Generates instruction definition JSON files.

    Args:
        xlen (str): RISC-V XLEN string (e.g. "32" or "64")
        extensions (list): List of supported extensions (e.g. "i", "v", "zicsr")
    """
    def __init__(self, xlen, extensions):
        self.xlen = xlen
        self.extensions = extensions
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
        xlen_str = "rv"+str(self.xlen)
        gen_path = os.path.join(xlen_str, 'gen')

        if not os.path.isdir(gen_path):
            os.makedirs(gen_path)

        for ext in self.extensions:
            filename = os.path.join(gen_path, "pegasus_uarch_" + xlen_str + ext.lower() + ".json")
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

def gen_supported_isa_header(arch_root, supported_rv64_exts, supported_rv32_exts):
    ''' Writes header file with ISA defines '''
    default_isa = "imafdcbv_zicsr_zifencei"

    rv64_json_files = [ f'    "{arch_root}/rv64/gen/pegasus_uarch_rv64{ext}.json"' for ext in supported_rv64_exts ]
    rv64_json_files_str = ', \\\n'.join( rv64_json_files )
    rv32_json_files = [ f'    "{arch_root}/rv32/gen/pegasus_uarch_rv32{ext}.json"' for ext in supported_rv32_exts ]
    rv32_json_files_str = ', \\\n'.join( rv32_json_files )

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
            if ext == 'a':
                fh.write( f'    \"zalrsc\", \\\n')
                fh.write( f'    \"zaamo\", \\\n')
            elif ext == 'm':
                fh.write( f'    \"zmmul\", \\\n')
            # Last extension in the list
            if idx == (len(supported_rv64_exts) - 1):
                fh.write( f'    \"{ext}\", \\\n')
                cd = ', \"zcd\"' if 'd' in supported_rv64_exts else ''
                fh.write( f'    \"c\"{cd} }}\n')
            else:
                fh.write( f'    \"{ext}\", \\\n')
        fh.write('#define SUPPORTED_RV32_EXTS {\\\n')
        for idx, ext in enumerate(supported_rv32_exts):
            if ext == 'a':
                fh.write( f'    \"zalrsc\", \\\n')
                fh.write( f'    \"zaamo\", \\\n')
            elif ext == 'm':
                fh.write( f'    \"zmmul\", \\\n')
            # Last extension in the list
            if idx == (len(supported_rv32_exts) - 1):
                fh.write( f'    \"{ext}\", \\\n')
                cd = ', \"zcd\"' if 'd' in supported_rv32_exts else ''
                cf = ', \"zcf\"' if 'f' in supported_rv32_exts else ''
                fh.write( f'    \"c\"{cd}{cf} }}\n')
            else:
                fh.write( f'    \"{ext}\", \\\n')
        fh.write( '#define RV64_UARCH_JSON_LIST {\\\n' )
        fh.write( rv64_json_files_str )
        fh.write( ' }\n' )
        fh.write( '#define RV32_UARCH_JSON_LIST {\\\n' )
        fh.write( rv32_json_files_str )
        fh.write( ' }\n' )

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

    supported_rv64_exts, supported_rv32_exts = get_supported_exts()

    if args.gen_supported_isa_header:
        gen_supported_isa_header(arch_root, supported_rv64_exts, supported_rv32_exts)
        return

    # Instruction uarch jsons
    if (args.xlen == "rv64"):
        inst_handler_gen = InstJSONGenerator("64", supported_rv64_exts)
        inst_handler_gen.write_jsons()
    else:
        inst_handler_gen = InstJSONGenerator("32", supported_rv32_exts)
        inst_handler_gen.write_jsons()


if __name__ == "__main__":
    main()
