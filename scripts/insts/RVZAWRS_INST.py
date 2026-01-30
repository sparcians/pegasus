RVZAWRS_MAVIS_EXTS = ["zawrs"]

RV32ZAWRS_INST = [
    {'mnemonic': 'wrs.nto', 'handler': 'wrs_nto', 'cost': 1, 'tags': 'ZAWRS_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'wrs.sto', 'handler': 'wrs_sto', 'cost': 1, 'tags': 'ZAWRS_EXT_32', 'memory': False, 'cof': False},
]

RV64ZAWRS_INST = [
    {'mnemonic': 'wrs.nto', 'handler': 'wrs_nto', 'cost': 1, 'tags': 'ZAWRS_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'wrs.sto', 'handler': 'wrs_sto', 'cost': 1, 'tags': 'ZAWRS_EXT_64', 'memory': False, 'cof': False},
]
