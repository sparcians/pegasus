RVH_MAVIS_EXTS = ["h"]

RV32H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_32', 'memory': False, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.b', 'handler': 'hlv.b', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.bu', 'handler': 'hlv.bu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.h', 'handler': 'hlv.h', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.hu', 'handler': 'hlv.hu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.w', 'handler': 'hlv.w', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlvx.hu', 'handler': 'hlvx.hu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlvx.wu', 'handler': 'hlvx.wu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.b', 'handler': 'hsv.b', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.h', 'handler': 'hsv.h', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.w', 'handler': 'hsv.w', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
]

RV64H_INST = [
    {'mnemonic': 'hfence.vvma', 'handler': 'hfence.vvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hfence.gvma', 'handler': 'hfence.gvma', 'cost': 1, 'tags': 'H_EXT_64', 'memory': False, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.b', 'handler': 'hlv.b', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.bu', 'handler': 'hlv.bu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.h', 'handler': 'hlv.h', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.hu', 'handler': 'hlv.hu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.w', 'handler': 'hlv.w', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.wu', 'handler': 'hlv.wu', 'cost': 1, 'tags': 'H_EXT_32', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlv.d', 'handler': 'hlv.d', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlvx.hu', 'handler': 'hlvx.hu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hlvx.wu', 'handler': 'hlvx.wu', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.b', 'handler': 'hsv.b', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.h', 'handler': 'hsv.h', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.w', 'handler': 'hsv.w', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
    {'mnemonic': 'hsv.d', 'handler': 'hsv.d', 'cost': 1, 'tags': 'H_EXT_64', 'memory': True, 'cof': False, 'hypervisor': True},
]
