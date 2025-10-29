RVB_MAVIS_EXTS = ["b", "zba", "zbb", "zbc", "zbs"]

RV32ZBA_INST = [
    {'mnemonic': 'sh1add', 'handler': 'sh1add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'sh2add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'sh3add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBA_INST = [
    {'mnemonic': 'add.uw', 'handler': 'add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add', 'handler': 'sh1add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add.uw', 'handler': 'sh1add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'sh2add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add.uw', 'handler': 'sh2add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'sh3add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add.uw', 'handler': 'sh3add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'slli.uw', 'handler': 'slli.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]

RV32ZBB_INST = [
    {'mnemonic': 'andn', 'handler': 'andn', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'clz', 'handler': 'clz', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'ctz', 'handler': 'ctz', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'cpop', 'handler': 'cpop', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'max', 'handler': 'max', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'maxu', 'handler': 'maxu', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'min', 'handler': 'min', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'minu', 'handler': 'minu', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'orc.b', 'handler': 'orc.b', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'orn', 'handler': 'orn', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'rev8', 'handler': 'rev8', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'rol', 'handler': 'rol', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'ror', 'handler': 'ror', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'rori', 'handler': 'rori', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sext.b', 'handler': 'sext.b', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sext.h', 'handler': 'sext.h', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'xnor', 'handler': 'xnor', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'zext.h', 'handler': 'zext.h', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBB_INST = [
    {'mnemonic': 'andn', 'handler': 'andn', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clz', 'handler': 'clz', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clzw', 'handler': 'clzw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'ctz', 'handler': 'ctz', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'ctzw', 'handler': 'ctzw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cpop', 'handler': 'cpop', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'cpopw', 'handler': 'cpopw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'max', 'handler': 'max', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'maxu', 'handler': 'maxu', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'min', 'handler': 'min', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'minu', 'handler': 'minu', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'orc.b', 'handler': 'orc.b', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'orn', 'handler': 'orn', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rev8', 'handler': 'rev8', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rol', 'handler': 'rol', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rolw', 'handler': 'rolw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'ror', 'handler': 'ror', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rori', 'handler': 'rori', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'roriw', 'handler': 'roriw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'rorw', 'handler': 'rorw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sext.b', 'handler': 'sext.b', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sext.h', 'handler': 'sext.h', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'xnor', 'handler': 'xnor', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'zext.h', 'handler': 'zext.h', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]

RV32ZBC_INST = [
    {'mnemonic': 'clmul', 'handler': 'clmul', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulh', 'handler': 'clmulh', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulr', 'handler': 'clmulr', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBC_INST = [
    {'mnemonic': 'clmul', 'handler': 'clmul', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulh', 'handler': 'clmulh', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulr', 'handler': 'clmulr', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]

RV32ZBS_INST = [
    {'mnemonic': 'bclr', 'handler': 'bclr', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'bclri', 'handler': 'bclri', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'bext', 'handler': 'bext', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'bexti', 'handler': 'bexti', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'binv', 'handler': 'binv', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'binvi', 'handler': 'binvi', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'bset', 'handler': 'bset', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'bseti', 'handler': 'bseti', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBS_INST = [
    {'mnemonic': 'bclr', 'handler': 'bclr', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'bclri', 'handler': 'bclri', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'bext', 'handler': 'bext', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'bexti', 'handler': 'bexti', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'binv', 'handler': 'binv', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'binvi', 'handler': 'binvi', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'bset', 'handler': 'bset', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'bseti', 'handler': 'bseti', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]
