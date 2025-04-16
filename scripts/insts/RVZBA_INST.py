RV32ZBA_INST = [
    {'mnemonic': 'sh1add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBA_INST = [
    {'mnemonic': 'add.uw', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add.uw', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add.uw', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add.uw', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'slli.uw', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]
