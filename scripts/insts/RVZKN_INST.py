# RVZKN_INST.py
# Combined instructions for ZKNE, ZKND and (ZKNE + ZKND) 

RVZKN_MAVIS_EXTS= ['zkne', 'zknd', 'zkne_zknd_common']


RV32ZKNE_INST = [
    {'mnemonic': 'aes32esi',  'handler': 'aes32esi',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'aes32esmi', 'handler': 'aes32esmi', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
]

RV64ZKNE_INST = [
    {'mnemonic': 'aes64es',  'handler': 'aes64es',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'aes64esm', 'handler': 'aes64esm', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
]

RV32ZKND_INST = [
    {'mnemonic': 'aes32dsi',  'handler': 'aes32dsi',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'aes32dsmi', 'handler': 'aes32dsmi', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_32', 'memory': False, 'cof': False},
]

RV64ZKND_INST = [
    {'mnemonic': 'aes64ds',  'handler': 'aes64ds',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'aes64dsm', 'handler': 'aes64dsm', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'aes64im',  'handler': 'aes64im',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
]

# zkne+zknd combined instructions
RV64ZKNE_ZKND_COMMON_INST = [
    {'mnemonic': 'aes64ks1i', 'handler': 'aes64ks1i', 'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'aes64ks2',  'handler': 'aes64ks2',  'cost': 1, 'tags': 'SCALAR_CRYPTO_EXT_64', 'memory': False, 'cof': False},
]
