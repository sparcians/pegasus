RV32ZICOND_INST = [
    {'mnemonic': 'czero.eqz', 'handler': 'nop', 'cost': 1, 'tags': 'ZICOND_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'czero.nez', 'handler': 'nop', 'cost': 1, 'tags': 'ZICOND_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICOND_INST = [
    {'mnemonic': 'czero.eqz', 'handler': 'nop', 'cost': 1, 'tags': 'ZICOND_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'czero.nez', 'handler': 'nop', 'cost': 1, 'tags': 'ZICOND_EXT_64', 'memory': False, 'cof': False},
]
