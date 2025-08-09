RV32ZFA_INST = [
    {'mnemonic': 'fli.s', 'handler': 'fli.s', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fli.d', 'handler': 'fli.d', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fminm.s', 'handler': 'fminm.s', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fmaxm.s', 'handler': 'fmaxm.s', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fround.s', 'handler': 'fround.s', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'froundnx.s', 'handler': 'froundnx.s', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'froundnx', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fcvtmod.w.d', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fleq.s', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'fltq.s', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False}
]

RV64ZFA_INST = [
    {'mnemonic': 'fli.s', 'handler': 'fli.s', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fli.d', 'handler': 'fli.d', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fminm.s', 'handler': 'fminm.s', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fmaxm.s', 'handler': 'fmaxm.s', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fround.s', 'handler': 'fround.s', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'froundnx.s', 'handler': 'froundnx.s', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'froundnx', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fcvtmod.w.d', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fleq.s', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'fltq.s', 'handler': 'nop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False}
]
