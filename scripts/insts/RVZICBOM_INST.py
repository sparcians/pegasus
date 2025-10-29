RVZICBOM_MAVIS_EXTS = ["zicbom"]

RV32ZICBOM_INST = [
    {'mnemonic': 'cbo.clean', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cbo.flush', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cbo.inval', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICBOM_INST = [
    {'mnemonic': 'cbo.clean', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cbo.flush', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cbo.inval', 'handler': 'nop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]
