#!/usr/bin/env python3
"""Helper script for regenerating register definition JSON files.
"""

import json
import os

from RV64_CSR import CSR64_DEFS
from RV32_CSR import CSR32_DEFS

from REG_CONSTS import CSR_CONSTS
from REG_CONSTS import ATLAS_INTERNAL_REGISTERS

CSR_BF_HEADER_FILE_NAME = "CSRBitMasks{reg_size}.hpp"
CSR_FI_HEADER_FILE_NAME = "CSRFieldIdxs{reg_size}.hpp"

CSR_HEADER_FILE_NAME = "CSRNums.hpp"

def GetCsrNumFileHeader(reg_size):
    header = '#pragma once\n'
    header += '\n'
    header += '#include <cinttypes>\n'
    if reg_size > 0:
        header += '#include "include/CSRFieldIdxs{reg_size}.hpp"\n'.format(reg_size=reg_size)
    header += '\n'
    header += '//\n'
    header += '// This is generated file from scripts/GenRegisterJSON.py\n'
    header += '//\n'
    header += '// DO NOT MODIFY THIS FILE BY HAND\n'
    header += '//\n'
    header += '\n'
    header += 'namespace atlas\n'
    header += '{\n'
    return header

CSR_NUM_FILE_CSRNUM_COMMENT = """
    // CSR Nums
"""

CSR_NUM_FILE_CONSTANT_COMMENT = """
    // Constants defined for RISC-V
"""

CSR_NUM_FILE_INTERNAL_REGS_COMMENT = """
    // Constants defined for RISC-V
"""

CSR_NUM_FILE_FOOTER = """
}
"""

def gen_csr_num_header():
    """Generate the CSR CPP header file
    """

    csr_header_file = open(CSR_HEADER_FILE_NAME, "w")
    csr_header_file.write(GetCsrNumFileHeader(-1))

    # TODO: Can we get keys in JSON to be printed in hex?
    CSR_DEFS = CSR32_DEFS.copy()
    CSR_DEFS.update(CSR64_DEFS)
    CONST = CSR_CONSTS

    csr_header_file.write(CSR_NUM_FILE_CSRNUM_COMMENT)
    csr_largest_value = -1;
    for k, v in CSR_DEFS.items():
        csr_header_file.write("    static constexpr uint32_t "+
                              (v[0]).upper()+
                              " = "+
                              str(hex(k))+
                              "; // "+
                              str(k)+
                              "\n")
        csr_largest_value = max(csr_largest_value, k)

    csr_header_file.write("    static constexpr uint32_t CSR_LARGEST_VALUE = "+
                          str(hex(csr_largest_value)) +
                          ";\n")

    csr_header_file.write(CSR_NUM_FILE_CONSTANT_COMMENT)
    for k, v in CONST.items():
        csr_header_file.write("    static constexpr uint64_t "+
                              k.upper()+
                              " = "+
                              str(hex(v[0]))+
                              "; // "+
                              str(v[1])+
                              "\n")

    csr_header_file.write(CSR_NUM_FILE_INTERNAL_REGS_COMMENT)
    for k, v in ATLAS_INTERNAL_REGISTERS.items():
        csr_header_file.write("    static constexpr uint64_t "+
                              k.upper()+
                              " = "+
                              str(hex(v[0]))+
                              "; // "+
                              str(v[1])+
                              "\n")

    csr_header_file.write(CSR_NUM_FILE_FOOTER)
    csr_header_file.close()
    return CSR_HEADER_FILE_NAME

def gen_csr_field_idxs_header(reg_size):
    """Generate the CSR header file with field indexes
    """
    data_width = reg_size*8
    csr_fi_header_file_name = CSR_FI_HEADER_FILE_NAME.format(reg_size=data_width)
    csr_fi_header_file = open(csr_fi_header_file_name, "w")
    csr_fi_header_file.write(GetCsrNumFileHeader(-1))

    if data_width == 32:
        json_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "arch", "rv32"))
    elif data_width == 64:
        json_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "arch", "rv64"))

    reg_csr_json_filename = os.path.join(json_dir, "reg_csr.json")
    reg_fp_json_filename  = os.path.join(json_dir, "reg_fp.json")
    reg_int_json_filename = os.path.join(json_dir, "reg_int.json")
    reg_vec_json_filename = os.path.join(json_dir, "reg_vec.json")

    def WriteRegisterFieldIdxs(reg_type, reg_json_filename, fout):
        with open(reg_json_filename) as reg_json_file:
            reg_json = json.load(reg_json_file)

        def GetWritableFieldsBitMask(reg_json):
            if 'fields' not in reg_json:
                return 0xffffffffffffffff
            
            if reg_type == 'FP':
                return 0xffffffffffffffff

            fields = reg_json['fields']
            bit_mask = 0
            for _, field_defn in fields.items():
                if not field_defn['readonly']:
                    low_bit = field_defn['low_bit']
                    high_bit = field_defn['high_bit']

                    # Set the bits from (low_bit, high_bit) in bit_mask to 1 (inclusive)
                    for i in range(low_bit, high_bit + 1):
                        bit_mask |= (1 << i)

            return bit_mask

        fout.write('\n')
        fout.write('    struct {}\n    {{\n\n'.format(reg_type))
        for reg in reg_json:
            # FP registers all have "sp" and "dp" fields
            if reg_type == 'FP':
                reg['fields']['sp'] = {'low_bit': 0, 'high_bit': 31, 'readonly': False}
                reg['fields']['dp'] = {'low_bit': 32, 'high_bit': 63, 'readonly': False}

            reg_names = [reg['name']]
            reg_names.extend(reg['aliases'])
            reg_num = reg['num']

            for i,reg_name in enumerate(reg_names):
                if i > 0:
                    fout.write('\n')

                fout.write('        // Register {} '.format(reg_name.upper()))
                if i > 0:
                    fout.write('(aliased to {}) \n'.format(reg_names[0].upper()))
                    fout.write('        using {} = {};\n\n'.format(reg_name.upper(), reg_names[0].upper()))
                    continue

                fout.write('\n')
                fout.write('        struct {} {{\n'.format(reg_name.upper()))
                fout.write('            static constexpr uint32_t reg_num = {};\n'.format(reg_num))
                writable_fields_bit_mask = GetWritableFieldsBitMask(reg)
                fout.write('            static constexpr uint64_t writable_fields_bit_mask = {};'.format(hex(writable_fields_bit_mask)))

                if 'fields' in reg:
                    for field_name, field_defn in reg['fields'].items():
                        fout.write('\n\n')
                        fout.write('            struct {} {{\n'.format(field_name.lower()))
                        fout.write('                static constexpr uint32_t low_bit = {};\n'.format(field_defn['low_bit']))
                        fout.write('                static constexpr uint32_t high_bit = {};\n'.format(field_defn['high_bit']))
                        fout.write('                static constexpr bool readonly = {};\n'.format(field_defn['readonly']).lower())
                        fout.write('            };')

                fout.write('\n        };\n')

        fout.write('\n    };\n')

    WriteRegisterFieldIdxs("CSR", reg_csr_json_filename, csr_fi_header_file)
    WriteRegisterFieldIdxs("FP",  reg_fp_json_filename,  csr_fi_header_file)
    WriteRegisterFieldIdxs("INT", reg_int_json_filename, csr_fi_header_file)
    WriteRegisterFieldIdxs("VEC", reg_vec_json_filename, csr_fi_header_file)

    csr_fi_header_file.write(CSR_NUM_FILE_FOOTER)
    csr_fi_header_file.close()
    return csr_fi_header_file_name

def gen_csr_bitmask_header(reg_size):
    """Generate the CSR header file with bitfields
    """
    data_width = reg_size*8
    csr_bf_header_file_name = CSR_BF_HEADER_FILE_NAME.format(reg_size=data_width)
    csr_bf_header_file = open(csr_bf_header_file_name, "w")
    csr_bf_header_file.write(GetCsrNumFileHeader(data_width))

    CSR_DEFS = CSR32_DEFS if (reg_size == 4) else CSR64_DEFS
    FILL_MASK = 0xffffffffffffffff
    SIZE = 64

    csr_keys_with_bitmasks = set()
    csr_keys_without_bitmasks = set()
    max_csr_num = max([int(k) for k in CSR_DEFS.keys()])

    for k, v in CSR_DEFS.items():
        # Print only if there are bit flags
        if v[2]:
            csr_bf_header_file.write("    namespace "+
                                     (v[0]).upper()+
                                     "_" + str(data_width) + "_bitmasks {\n")
            fields = v[2]
            for field_name in fields:
                if field_name.lower() != "resv" and field_name.lower() != "wpri":
                    high_bit = fields[field_name]['high_bit']
                    low_bit = fields[field_name]['low_bit']
                    mask = (FILL_MASK >> (SIZE - (high_bit - low_bit + 1))) << low_bit
                    csr_bf_header_file.write("        static constexpr uint64_t " +
                                             field_name + " = " + hex(mask) + ";\n")

            csr_bf_header_file.write("    } // namespace "+
                                     (v[0]).upper()+
                                     "_" + str(data_width) + "_bitfield\n\n")

            csr_keys_with_bitmasks.add(k)
        else:
            csr_keys_without_bitmasks.add(k)

    csr_bf_header_file.write('\n\n')
    csr_bf_header_file.write('    inline uint64_t GetCSRBitMask(uint32_t reg_num, uint32_t field_idx) {\n')
    csr_bf_header_file.write('        static std::vector<std::vector<uint64_t>> csr_bitmasks;\n')
    csr_bf_header_file.write('        if (csr_bitmasks.empty()) {\n')
    csr_bf_header_file.write('            csr_bitmasks.resize({});\n\n'.format(max_csr_num+1))

    for k, v in CSR_DEFS.items():
        if k not in csr_keys_with_bitmasks:
            continue

        csr_num = int(k)
        csr_name = v[0].upper()

        csr_bf_header_file.write('            // {}\n'.format(csr_name))
        csr_bf_header_file.write('            csr_bitmasks[{}] = {{\n'.format(csr_num))
        fields = v[2]
        bitmasks = []
        for field_name in fields:
            if field_name.lower() != "resv" and field_name.lower() != "wpri":
                namespace = '{}_{}_bitmasks'.format(csr_name, data_width)
                bitmasks.append('{}::{}'.format(namespace, field_name))

        bitmasks = ['                ' + mask for mask in bitmasks]
        csr_bf_header_file.write(',\n'.join(bitmasks))
        csr_bf_header_file.write('\n            };\n\n')

    csr_bf_header_file.write('        }\n\n')
    csr_bf_header_file.write('        #ifdef NDEBUG\n')
    csr_bf_header_file.write('            const auto& bitmasks = csr_bitmasks[reg_num];\n')
    csr_bf_header_file.write('            if (bitmasks.empty()) {\n')
    csr_bf_header_file.write('                return 0xFFFFFFFFFFFFFFFF;\n')
    csr_bf_header_file.write('            }\n')
    csr_bf_header_file.write('            return bitmasks[field_idx];\n')
    csr_bf_header_file.write('        #else\n')
    csr_bf_header_file.write('            const auto& bitmasks = csr_bitmasks.at(reg_num);\n')
    csr_bf_header_file.write('            if (bitmasks.empty()) {\n')
    csr_bf_header_file.write('                return 0xFFFFFFFFFFFFFFFF;\n')
    csr_bf_header_file.write('            }\n')
    csr_bf_header_file.write('            return bitmasks.at(field_idx);\n')
    csr_bf_header_file.write('        #endif\n')
    csr_bf_header_file.write('    }\n')

    csr_bf_header_file.write(CSR_NUM_FILE_FOOTER)
    csr_bf_header_file.close()
    return csr_bf_header_file_name
