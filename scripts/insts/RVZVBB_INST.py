# ZVBB Extension
# Vector Basic Bit-manipulation

# Mnemonic          Instruction
# vandn.[vv,vx]     Vector And-Not
# vbrev.v           Vector Reverse Bits in Elements
# vbrev8.v          Vector Reverse Bits in Bytes
# vrev8.v           Vector Reverse Bytes
# vclz.v            Vector Count Leading Zeros
# vctz.v            Vector Count Trailing Zeros
# vcpop.v           Vector Population Count
# vrol.[vv,vx]      Vector Rotate Left
# vror.[vv,vx,vi]   Vector Rotate Right
# vwsll.[vv,vx,vi]  Vector Widening Shift Left Logical

RVZVBB_MAVIS_EXTS = ['zvbb']

RV32ZVBB_INST = [
    {'mnemonic': 'vandn.vv', 'handler': 'vandn.vv', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vandn.vx', 'handler': 'vandn.vx', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vbrev.v' , 'handler': 'vbrev.v' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vbrev8.v', 'handler': 'vbrev8.v', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vrev8.v' , 'handler': 'vrev8.v' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vclz.v'  , 'handler': 'vclz.v'  , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vctz.v'  , 'handler': 'vctz.v'  , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vcpop.v' , 'handler': 'vcpop.v' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vrol.vv' , 'handler': 'vrol.vv' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vrol.vx' , 'handler': 'vrol.vx' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vv' , 'handler': 'vror.vv' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vx' , 'handler': 'vror.vx' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vi' , 'handler': 'vror.vi' , 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vv', 'handler': 'vwsll.vv', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vx', 'handler': 'vwsll.vx', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vi', 'handler': 'vwsll.vi', 'cost': 1, 'tags': 'ZVBB_EXT_32', 'memory': False, 'cof': False},
]

RV64ZVBB_INST = [
    {'mnemonic': 'vandn.vv', 'handler': 'vandn.vv', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vandn.vx', 'handler': 'vandn.vx', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vbrev.v' , 'handler': 'vbrev.v' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vbrev8.v', 'handler': 'vbrev8.v', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vrev8.v' , 'handler': 'vrev8.v' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vclz.v'  , 'handler': 'vclz.v'  , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vctz.v'  , 'handler': 'vctz.v'  , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vcpop.v' , 'handler': 'vcpop.v' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vrol.vv' , 'handler': 'vrol.vv' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vrol.vx' , 'handler': 'vrol.vx' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vv' , 'handler': 'vror.vv' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vx' , 'handler': 'vror.vx' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vror.vi' , 'handler': 'vror.vi' , 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vv', 'handler': 'vwsll.vv', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vx', 'handler': 'vwsll.vx', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'vwsll.vi', 'handler': 'vwsll.vi', 'cost': 1, 'tags': 'ZVBB_EXT_64', 'memory': False, 'cof': False},
]
