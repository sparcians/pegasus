RVZICBOP_MAVIS_EXTS = ["zicbop"]

RV32ZICBOP_INST = [
    {'mnemonic': 'prefetch.i', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'prefetch.r', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'prefetch.w', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICBOP_INST = [
    {'mnemonic': 'prefetch.i', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'prefetch.r', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'prefetch.w', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
]
