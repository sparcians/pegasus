RVZIHINTNTL_MAVIS_EXTS = ["zihintntl"]

RV32ZIHINTNTL_INST = [
  {'mnemonic': 'NTL.P1'    , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x2
  {'mnemonic': 'NTL.PALL'  , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x3
  {'mnemonic': 'NTL.S1'    , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x4
  {'mnemonic': 'NTL.ALL'   , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x5
]

RV64ZIHINTNTL_INST = [
  {'mnemonic': 'NTL.P1'    , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x2
  {'mnemonic': 'NTL.PALL'  , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x3
  {'mnemonic': 'NTL.S1'    , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x4
  {'mnemonic': 'NTL.ALL'   , 'handler': 'nop', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False}, # add x0,x0,x5
]

