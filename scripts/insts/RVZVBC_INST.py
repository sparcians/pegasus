RVZVBC_MAVIS_EXTS = ["zvbc"]

RV32ZVBC_INST = [
    {'mnemonic': 'vclmul.vv', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmul.vx', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmulh.vv', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmulh.vx', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_32', 'memory': False, 'cof': False},
]

RV64ZVBC_INST = [
    {'mnemonic': 'vclmul.vv', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmul.vx', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmulh.vv', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vclmulh.vx', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZVBC_EXT_64', 'memory': False, 'cof': False},
]
