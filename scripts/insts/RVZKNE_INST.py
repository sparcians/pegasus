RVZKNE_MAVIS_EXTS=["zkne"]

RV32ZKNE_INST = [
    {'mnemonic' : 'aes32esi', 'handler' : 'aes322esi',  'cost': 1, 'tags'  : 'SCALAR_CRYPTO_EXT_32', 'memory' : False, 'cof' : False  },
    {'mnemonic' : 'aes32esmi', 'handler' : 'aes32esmi', 'cost' : 1, 'tags' : 'SCALAR_CRYPTO_EXT_32', 'memory' : False, 'cof' : False}
]

RV64ZKNE_INST = [
    {'mnemonic' : 'aes64es', 'handler' : 'aes64es', 'cost' : 1, 'tags' : 'SCALAR_CRYPTO_EXT_64', 'memory' : False, 'cof' : False},
    {'mnemonic' : 'aes64esm', 'handler' : 'aes64esm', 'cost' : 1, 'tags' : 'SCALAR_CRYPTO_EXT_64', 'memory' : False, 'cof' : False}
]