RVSVINVAL_MAVIS_EXTS = ["svinval", "svinval_h"]

RV32SVINVAL_INST = [
    {'mnemonic': 'sinval.vma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sfence.w.inval', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.vvma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_H_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.gvma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_H_EXT_32', 'memory': False, 'cof': False},
]

RV64SVINVAL_INST = [
    {'mnemonic': 'sinval.vma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sfence.w.inval', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.vvma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_H_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'hinval.gvma', 'handler': 'unsupported', 'cost': 1, 'tags': 'SVINVAL_H_EXT_64', 'memory': False, 'cof': False},
]
