# RISC-V Zbkx (Crossbar permutation) Extension Instructions
# Part of the RISC-V Scalar Cryptography Extension

#   RV32 RV64 OTHER     Mnemonic    Instruction
#   x    x    Zbkx      XPERM8      Byte-wise lookup of indices into a vector
#   x    x    Zbkx      XPERM4      Nibble-wise lookup of indices into a vector

RVZBKX_MAVIS_EXTS = ["zbkx"]
RV32ZBKX_INST = [
  {'mnemonic': 'xperm8', 'handler': 'xperm8', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
  {'mnemonic': 'xperm4', 'handler': 'xperm4', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBKX_INST = [
  {'mnemonic': 'xperm8', 'handler': 'xperm8', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
  {'mnemonic': 'xperm4', 'handler': 'xperm4', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
]