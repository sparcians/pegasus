RVSMRNMI_MAVIS_EXTS = ["smrnmi"]

RV32SMRNMI_INST = [
    {'mnemonic': 'mnret', 'handler': 'unsupported', 'cost': 1, 'tags': 'SMRNMI_EXT_32', 'memory': False, 'cof': True, 'return': True},
]

RV64SMRNMI_INST = [
    {'mnemonic': 'mnret', 'handler': 'unsupported', 'cost': 1, 'tags': 'SMRNMI_EXT_64', 'memory': False, 'cof': True, 'return': True},
]
