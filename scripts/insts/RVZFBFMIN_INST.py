RVZFBFMIN_MAVIS_EXTS = ["zfbfmin"]

RV32ZFBFMIN_INST = [
    {'mnemonic': 'fcvt.bf16.s', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZFBFMIN_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fcvt.s.bf16', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZFBFMIN_EXT_32', 'memory': False, 'cof': False},
]

RV64ZFBFMIN_INST = [
    {'mnemonic': 'fcvt.bf16.s', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZFBFMIN_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fcvt.s.bf16', 'handler': 'unsupported', 'cost': 1, 'tags': 'ZFBFMIN_EXT_64', 'memory': False, 'cof': False},
]
