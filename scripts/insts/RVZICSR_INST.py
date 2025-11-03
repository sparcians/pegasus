RVZICSR_MAVIS_EXTS = ["zicsr"]

RV32ZICSR_INST = [
    {'mnemonic': 'csrrw', 'handler': 'csrrw', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrs', 'handler': 'csrrs', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrc', 'handler': 'csrrc', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrwi', 'handler': 'csrrwi', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrsi', 'handler': 'csrrsi', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrci', 'handler': 'csrrci', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICSR_INST = [
    {'mnemonic': 'csrrw', 'handler': 'csrrw', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrs', 'handler': 'csrrs', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrc', 'handler': 'csrrc', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrwi', 'handler': 'csrrwi', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrsi', 'handler': 'csrrsi', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrci', 'handler': 'csrrci', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
]
