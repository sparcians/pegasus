RVH_MAVIS_EXTS = ["h"]

RV32H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hlv.b', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.bu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.h', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.hu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.w', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.hu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.wu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.b', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.h', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.w', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
]

RV64H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hlv.b', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.bu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.h', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.hu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.w', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.wu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.d', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.hu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.wu', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.b', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.h', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.w', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.d', 'handler': 'unsupported', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
]
