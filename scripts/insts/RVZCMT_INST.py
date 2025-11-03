
#   RV32    RV64    Mnemonic    Instruction
#   x       x       cm.jt       index Jump via table
#   x       x       cm.jalt     index Jump and link via table

RVZCMT_MAVIS_EXTS = ["zcmt"]

RV32ZCMT_INST = [
    {'mnemonic': 'cm.jt'  , 'handler': 'cm_jt'  , 'cost': 1, 'tags': 'ZCMT_EXT_32', 'memory': True, 'cof': True},
    {'mnemonic': 'cm.jalt', 'handler': 'cm_jalt', 'cost': 1, 'tags': 'ZCMT_EXT_32', 'memory': True, 'cof': True},
]

RV64ZCMT_INST = [
    {'mnemonic': 'cm.jt'  , 'handler': 'cm_jt'  , 'cost': 1, 'tags': 'ZCMT_EXT_64', 'memory': True, 'cof': True},
    {'mnemonic': 'cm.jalt', 'handler': 'cm_jalt', 'cost': 1, 'tags': 'ZCMT_EXT_64', 'memory': True, 'cof': True},
]
