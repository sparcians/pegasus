RV32ZILSD_MAVIS_EXTS = ["zilsd"]

RV32ZILSD_INST = [
    {'mnemonic': 'ld', 'handler': 'ld', 'cost': 1, 'tags': 'ZILSD_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'sd', 'handler': 'sd', 'cost': 1, 'tags': 'ZILSD_EXT_32', 'memory': True, 'cof': False},
]
