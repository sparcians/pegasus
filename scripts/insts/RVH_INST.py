RVH_MAVIS_EXTS = ["h"]

RV32H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hlv.b', 'handler': 'hlv.b', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.bu', 'handler': 'hlv.bu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.h', 'handler': 'hlv.h', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.hu', 'handler': 'hlv.hu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.w', 'handler': 'hlv.w', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.hu', 'handler': 'hlvx.hu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.wu', 'handler': 'hlvx.wu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.b', 'handler': 'hsv.b', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.h', 'handler': 'hsv.h', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.w', 'handler': 'hsv.w', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
]

RV64H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hlv.b', 'handler': 'hlv.b', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.bu', 'handler': 'hlv.bu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.h', 'handler': 'hlv.h', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.hu', 'handler': 'hlv.hu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.w', 'handler': 'hlv.w', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.wu', 'handler': 'hlv.wu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'hlv.d', 'handler': 'hlv.d', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.hu', 'handler': 'hlvx.hu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hlvx.wu', 'handler': 'hlvx.wu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.b', 'handler': 'hsv.b', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.h', 'handler': 'hsv.h', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.w', 'handler': 'hsv.w', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'hsv.d', 'handler': 'hsv.d', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False},
]
