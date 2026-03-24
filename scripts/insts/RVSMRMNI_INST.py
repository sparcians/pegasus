RVSVINVAL_MAVIS_EXTS = ["smrmni"]

RV32SVINVAL_INST = [
    {'mnemonic': 'mnret', 'handler': 'unsupported', 'cost': 1, 'tags': 'SMRMNI_EXT_32', 'memory': False, 'cof': True},
]

RV64SVINVAL_INST = [
    {'mnemonic': 'mnret', 'handler': 'unsupported', 'cost': 1, 'tags': 'SMRMNI_EXT_64', 'memory': False, 'cof': True},
]
