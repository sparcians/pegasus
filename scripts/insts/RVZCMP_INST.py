RVZCMP_MAVIS_EXTS = ["zcmp"]

RV32ZCMP_INST = [
    {'mnemonic': 'cm.push', 'handler': 'cm.push', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.pop', 'handler': 'cm.pop', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.popretz', 'handler': 'cm.popretz', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.popret', 'handler': 'cm.popret', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.mvsa01', 'handler': 'cm.mvsa01', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.mva01s', 'handler': 'cm.mva01s', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False},
]

RV64ZCMP_INST = [
    {'mnemonic': 'cm.push', 'handler': 'cm.push', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.pop', 'handler': 'cm.pop', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.popretz', 'handler': 'cm.popretz', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.popret', 'handler': 'cm.popret', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.mvsa01', 'handler': 'cm.mvsa01', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cm.mva01s', 'handler': 'cm.mva01s', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False},
]
