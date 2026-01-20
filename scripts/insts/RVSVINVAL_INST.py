RVSVINVAL_MAVIS_EXTS = ["svinval"]

RV32SVINVAL_INST = [
    {'mnemonic': 'sinval.vma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sfence.w.inval', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.vvma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.gvma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False}
]

RV64SVINVAL_INST = [
    {'mnemonic': 'sinval.vma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sfence.w.inval', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.vvma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.gvma', 'handler': 'nop', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False}
]
