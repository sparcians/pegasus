RVZICOND_MAVIS_EXTS = ["zicond"]

RV32ZICOND_INST = [
    {'mnemonic': 'czero.eqz', 'handler': 'czero.eqz', 'cost': 1, 'tags': 'ZICOND_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'czero.nez', 'handler': 'czero.nez', 'cost': 1, 'tags': 'ZICOND_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICOND_INST = [
    {'mnemonic': 'czero.eqz', 'handler': 'czero.eqz', 'cost': 1, 'tags': 'ZICOND_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'czero.nez', 'handler': 'czero.nez', 'cost': 1, 'tags': 'ZICOND_EXT_64', 'memory': False, 'cof': False},
]
