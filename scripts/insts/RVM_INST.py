RVM_MAVIS_EXTS = ["m", "zmmul"]

RV32M_INST = [
    {'mnemonic': 'mul', 'handler': 'mul', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'mulh', 'handler': 'mulh', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'mulhsu', 'handler': 'mulhsu', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'mulhu', 'handler': 'mulhu', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'div', 'handler': 'div', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'divu', 'handler': 'divu', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'rem', 'handler': 'rem', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'remu', 'handler': 'remu', 'cost': 1, 'tags': 'M_EXT_32', 'memory': False, 'cof': False},
]

RV64M_INST = [
    {'mnemonic': 'mul', 'handler': 'mul', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'mulh', 'handler': 'mulh', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'mulhsu', 'handler': 'mulhsu', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'mulhu', 'handler': 'mulhu', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'div', 'handler': 'div', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'divu', 'handler': 'divu', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rem', 'handler': 'rem', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'remu', 'handler': 'remu', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'mulw', 'handler': 'mulw', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'divw', 'handler': 'divw', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'divuw', 'handler': 'divuw', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'remw', 'handler': 'remw', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'remuw', 'handler': 'remuw', 'cost': 1, 'tags': 'M_EXT_64', 'memory': False, 'cof': False},
]
