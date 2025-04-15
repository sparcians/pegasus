RV32ZICSR_INST = [
    {'mnemonic': 'csrrw', 'handler': 'csrrw_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrs', 'handler': 'csrrs_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrc', 'handler': 'csrrc_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrwi', 'handler': 'csrrwi_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrsi', 'handler': 'csrrsi_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrci', 'handler': 'csrrci_32', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]

RV64ZICSR_INST = [
    {'mnemonic': 'csrrw', 'handler': 'csrrw_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrs', 'handler': 'csrrs_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrc', 'handler': 'csrrc_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrwi', 'handler': 'csrrwi_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrsi', 'handler': 'csrrsi_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'csrrci', 'handler': 'csrrci_64', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
]
