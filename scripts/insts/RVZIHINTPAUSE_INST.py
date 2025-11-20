RVZIHINTPAUSE_MAVIS_EXTS = ["zihintpause"]

RV32ZIHINTPAUSE_INST = [
  {'mnemonic': 'pause'  , 'handler': 'pause', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False},   # fence p=w,s=0,rd=x0,rs1=x0    #TODO nop handler should be changed to a pause once multithread is supported
]

RV64ZIHINTPAUSE_INST = [
  {'mnemonic': 'pause'  , 'handler': 'pause', 'cost': 1, 'tags': 'HINT_EXT_32,HINT_EXT_64', 'memory': False, 'cof': False},   # fence p=w,s=0,rd=x0,rs1=x0    #TODO nop handler should be changed to a pause once multithread is supported
]
