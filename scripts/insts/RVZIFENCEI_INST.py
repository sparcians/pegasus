RV32ZIFENCEI_INST = [
    {'mnemonic': 'fence.i', 'handler': 'fence_i', 'cost': 1, 'tags': 'BASE_EXT_32', 'memory': False, 'cof': False, 'vdeleg': False, 'vector': False, 'float': False, 'maskable': False, 'segment': False, 'indexed': False, 'atomic': False, 'crypto': False},
]

RV64ZIFENCEI_INST = [
    {'mnemonic': 'fence.i', 'handler': 'fence_i', 'cost': 1, 'tags': 'BASE_EXT_64', 'memory': False, 'cof': False, 'vdeleg': False, 'vector': False, 'float': False, 'maskable': False, 'segment': False, 'indexed': False, 'atomic': False, 'crypto': False},
]
