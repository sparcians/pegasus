#   RV32 RV64 OTHER     Mnemonic    Instruction
#   x	 x    Zbb       ANDN 	    AND with inverted operand
#   x	 x    Zbb       ORN 	    OR with inverted operand
#   x	 x    -         PACK 	    Pack low halves of registers
#   x	 x    -         PACKH 	    Pack low bytes of registers
#   -    x    -         PACKW 	    Pack low 16-bits of registers (RV64)
#   x	 x    Zbb        REV8 	    Byte-reverse register
#   x	 x    -         BREV8 	    Reverse bits in bytes
#   x    x    Zbb       ROL 	    Rotate left (Register)
#   -    x    Zbb       ROLW 	    Rotate Left Word (Register)
#   x	 x    Zbb       ROR 	    Rotate right (Register)
#   x	 x    Zbb       RORI 	    Rotate right (Immediate)
#   -    x    Zbb       RORIW 	    Rotate right Word (Immediate)
#   -    x    Zbb       RORW 	    Rotate right Word (Register)
#   x	 -    -         UNZIP 	    Bit deinterleave
#   x	 x    Zbb       XNOR 	    Exclusive NOR
#   x	 -    -         ZIP 	    Bit interleave

RVZBKB_MAVIS_EXTS = ["zbkb"]

RV32ZBKB_INST = [
  {'mnemonic': 'andn'   , 'handler': 'andn'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'brev8'  , 'handler': 'brev8' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'orn'    , 'handler': 'orn'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'pack'   , 'handler': 'pack'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'packh'  , 'handler': 'packh' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'rev8'   , 'handler': 'rev8'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'rol'    , 'handler': 'rol'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'ror'    , 'handler': 'ror'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'rori'   , 'handler': 'rori'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'unzip'  , 'handler': 'unzip' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'xnor'   , 'handler': 'xnor'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'zip'    , 'handler': 'zip'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBKB_INST = [
  {'mnemonic': 'andn'   , 'handler': 'andn'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'brev8'  , 'handler': 'brev8' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'orn'    , 'handler': 'orn'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'pack'   , 'handler': 'pack'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'packh'  , 'handler': 'packh' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'packw'  , 'handler': 'packw' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'rev8'   , 'handler': 'rev8'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'rol'    , 'handler': 'rol'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'rolw'   , 'handler': 'rolw'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'ror'    , 'handler': 'ror'   , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'rori'   , 'handler': 'rori'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'roriw'  , 'handler': 'roriw' , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'rorw'   , 'handler': 'rorw'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'xnor'   , 'handler': 'xnor'  , 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
]
