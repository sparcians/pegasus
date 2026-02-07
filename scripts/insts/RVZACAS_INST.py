RVZACAS_MAVIS_EXTS = ["zacas"]

RV32ZACAS_INST = [
    {'mnemonic': 'amocas.w', 'handler': 'amocas_w', 'cost': 1, 'tags': 'A_EXT_32', 'memory': True, 'cof': False},
    {'mnemonic': 'amocas_d', 'handler': 'amocas_d', 'cost': 1, 'tags': 'A_EXT_32', 'memory': True, 'cof': False},
]

RV64ZACAS_INST = [
    {'mnemonic': 'amocas.w', 'handler': 'amocas_w', 'cost': 1, 'tags': 'A_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'amocas_d', 'handler': 'amocas_d', 'cost': 1, 'tags': 'A_EXT_64', 'memory': True, 'cof': False},
    {'mnemonic': 'amocas_q', 'handler': 'amocas_q', 'cost': 1, 'tags': 'A_EXT_64', 'memory': True, 'cof': False},
]
