#   RV32 RV64 OTHER     Mnemonic    Instruction
#   x	 x    Zbb       ANDN 	    AND with inverted operand
#   x	 x    -         BREV8 	    Reverse bits in bytes
#   x	 x    Zbb       ORN 	    OR with inverted operand
#   x	 x    -         PACK 	    Pack low halves of registers
#   x	 x    -         PACKH 	    Pack low bytes of registers
#   	 x    -         PACKW 	    Pack low 16-bits of registers (RV64)
#   x	 x    -         REV8 	    Byte-reverse register
#   x    x    Zbb       ROL 	    Rotate left (Register)
#   	 x    Zbb       ROLW 	    Rotate Left Word (Register)
#   x	 x    Zbb       ROR 	    Rotate right (Register)
#   x	 x    Zbb       RORI 	    Rotate right (Immediate)
#   	 x    Zbb       RORIW 	    Rotate right Word (Immediate)
#   	 x    Zbb       RORW 	    Rotate right Word (Register)
#   x	      -         UNZIP 	    Bit deinterleave
#   x	 x    Zbb       XNOR 	    Exclusive NOR
#   x	      -         ZIP 	    Bit interleave


RV32ZBKB_INST = [
  {'mnemonic': 'ANDN'   , 'handler': 'andn'  , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'BREV8'  , 'handler': 'brev8' , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'ORN'    , 'handler': 'orn'   , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'PACK'   , 'handler': 'pack'  , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'PACKH'  , 'handler': 'packh' , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'REV8'   , 'handler': 'rev8'  , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'ROL'    , 'handler': 'rol'   , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'ROR'    , 'handler': 'ror'   , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'RORI'   , 'handler': 'rori'  , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'UNZIP'  , 'handler': 'unzip' , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'XNOR'   , 'handler': 'xnor'  , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'ZIP'    , 'handler': 'zip'   , 'cost': 1, 'tags': 'CRYPT_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBKB_INST = [
  {'mnemonic': 'ANDN'   , 'handler': 'andn'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'BREV8'  , 'handler': 'brev8' , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'ORN'    , 'handler': 'orn'   , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'PACK'   , 'handler': 'pack'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'PACKH'  , 'handler': 'packh' , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'PACKW'  , 'handler': 'packw' , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'REV8'   , 'handler': 'rev8'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'ROL'    , 'handler': 'rol'   , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'ROLW'   , 'handler': 'rolw'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'ROR'    , 'handler': 'ror'   , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'RORI'   , 'handler': 'rori'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'RORIW'  , 'handler': 'roriw' , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'RORW'   , 'handler': 'rorw'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'XNOR'   , 'handler': 'xnor'  , 'cost': 1, 'tags': 'CRYPT_EXT_64', 'memory': False, 'cof': False},
]
