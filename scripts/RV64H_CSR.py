#

CSR64H_DEFS = {
    # Hypervisor Trap Setup
    0x600: {
             'name': 'hstatus',
             'desc': 'Hypervisor status register',
             'fields': {
               'WPRI'  : { 'high_bit':  4, 'low_bit':   0, 'readonly': True , 'desc': 'reserved' },
               'VSXL'  : { 'high_bit': 33, 'low_bit':  32, 'readonly': True , 'desc': 'Controls XLEN value for VS-mode' },
               'VTSR'  : { 'high_bit': 22, 'low_bit':  22, 'readonly': False, 'desc': 'Virtual Trap SRET' },
               'VTW'   : { 'high_bit': 21, 'low_bit':  21, 'readonly': False, 'desc': 'Virtual Timeout Wait' },
               'VTVM'  : { 'high_bit': 20, 'low_bit':  20, 'readonly': False, 'desc': 'Virtual Trap Virtual Memory' },
               'VGEIN' : { 'high_bit': 17, 'low_bit':  12, 'readonly': False, 'desc': 'Virtual Guest External Interrupt Number' },
               'HU'    : { 'high_bit':  9, 'low_bit':   9, 'readonly': False, 'desc': 'Hypervisor in U-mode' },
               'SPVP'  : { 'high_bit':  8, 'low_bit':   8, 'readonly': False, 'desc': 'Supervisor Previous Virtual Privilege' },
               'SPV'   : { 'high_bit':  7, 'low_bit':   7, 'readonly': False, 'desc': 'Supervisor Previous Virtualization mode' },
               'GVA'   : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Guest Virtual Address' },
               'VSBE'  : { 'high_bit':  5, 'low_bit':   5, 'readonly': True , 'desc': 'Controls endianness of explicit memory accesses made from VS-mode' },
             },
             'context': 'HART',
           },
    0x602: {
             'name': 'hedeleg',
             'desc': 'Hypervisor exception delegation register',
             'fields': {
               'WPRI'     : { 'high_bit': 14, 'low_bit':  14, 'readonly': True , 'desc': 'reserved' },
               'SGPF'     : { 'high_bit': 23, 'low_bit':  23, 'readonly': True , 'desc': 'Store/AMO guest-page fault' },
               'VIRTINST' : { 'high_bit': 22, 'low_bit':  22, 'readonly': True , 'desc': 'Virtual instruction' },
               'LGPF'     : { 'high_bit': 21, 'low_bit':  21, 'readonly': True , 'desc': 'Load guest-page fault' },
               'IGPF'     : { 'high_bit': 20, 'low_bit':  20, 'readonly': True , 'desc': 'Instruction guest-page fault' },
               'MECALL'   : { 'high_bit': 11, 'low_bit':  11, 'readonly': True , 'desc': 'Machine-mode environment call' },
               'VSECALL'  : { 'high_bit': 10, 'low_bit':  10, 'readonly': True , 'desc': 'Virtual supervisor-mode environment call' },
               'HSECALL'  : { 'high_bit':  9, 'low_bit':   9, 'readonly': True , 'desc': 'Supervisor-mode environment call' },
             },
             'context': 'HART',
           },
    0x603: {
             'name': 'hideleg',
             'desc': 'Hypervisor interrupt delegation register',
             'fields': {
               'WPRI' : { 'high_bit': 63, 'low_bit':  13, 'readonly': True , 'desc': 'reserved' },
               'SGEI' : { 'high_bit': 12, 'low_bit':  12, 'readonly': True , 'desc': 'Supervisor guest external interrupts' },
               'MEI'  : { 'high_bit': 11, 'low_bit':  11, 'readonly': True , 'desc': 'Machine-mode external interrupts' },
               'VSEI' : { 'high_bit': 10, 'low_bit':  10, 'readonly': False, 'desc': 'Virtuall supervisor-mode external interrupts' },
               'SEI'  : { 'high_bit':  9, 'low_bit':   9, 'readonly': True , 'desc': 'Supervisor-mode external interrupts' },
               'UEI'  : { 'high_bit':  8, 'low_bit':   8, 'readonly': True , 'desc': 'User-mode external interrupts' },
               'MTI'  : { 'high_bit':  7, 'low_bit':   7, 'readonly': True , 'desc': 'Machine-mode timer interrupts' },
               'VSTI' : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Virtuall supervisor-mode timer interrupts' },
               'STI'  : { 'high_bit':  5, 'low_bit':   5, 'readonly': True , 'desc': 'Supervisor-mode timer interrupts' },
               'UTI'  : { 'high_bit':  4, 'low_bit':   4, 'readonly': True , 'desc': 'User-mode timer interrupts' },
               'MSI'  : { 'high_bit':  3, 'low_bit':   3, 'readonly': True , 'desc': 'Machine-mode software interrupts' },
               'VSSI' : { 'high_bit':  2, 'low_bit':   2, 'readonly': False, 'desc': 'Virtual supervisor-mode software interrupts' },
               'SSI'  : { 'high_bit':  1, 'low_bit':   1, 'readonly': True , 'desc': 'Supervisor-mode software interrupts' },
               'USI'  : { 'high_bit':  0, 'low_bit':   0, 'readonly': True , 'desc': 'User-mode software interrupts' },
             },
             'context': 'HART',
           },
    0x604: {
             'name': 'hie',
             'desc': 'Hypervisor interrupt-enable register',
             'fields': {
               'WPRI'  : { 'high_bit': 63, 'low_bit':  13, 'readonly': True , 'desc': 'reserved' },
               'SGEIE' : { 'high_bit': 12, 'low_bit':  12, 'readonly': False, 'desc': 'Enable supervisor guest external interrupts' },
               'VSEIE' : { 'high_bit': 10, 'low_bit':  10, 'readonly': False, 'desc': 'Enable virtuall supervisor-mode external interrupts' },
               'VSTIE' : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Enable virtuall supervisor-mode timer interrupts' },
               'VSSIE' : { 'high_bit':  2, 'low_bit':   2, 'readonly': False, 'desc': 'Enable virtual supervisor-mode software interrupts' },
             },
             'context': 'HART',
           },
    0x606: {
             'name': 'hcounteren',
             'desc': 'Hypervisor counter-enable register',
             'fields': {},
             'context': 'HART',
           },
    0x607: {
             'name': 'hgeie',
             'desc': 'Hypervisor guest external interrupt-enable register',
             'fields': {},
             'context': 'HART',
           },

    # Hypervisor Trap Handling
    0x643: {
             'name': 'htval',
             'desc': 'Hypervisor trap value register',
             'fields': {},
             'context': 'HART',
           },
    0x644: {
             'name': 'hip',
             'desc': 'Hypervisor interrupt-pending register',
             'fields': {
               'WPRI'  : { 'high_bit': 63, 'low_bit':  13, 'readonly': True , 'desc': 'reserved' },
               'SGEIP' : { 'high_bit': 12, 'low_bit':  12, 'readonly': False, 'desc': 'Supervisor guest external interrupt pending' },
               'VSEIP' : { 'high_bit': 10, 'low_bit':  10, 'readonly': False, 'desc': 'Virtual supervisor-mode external interrupt pending' },
               'VSTIP' : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Virtual supervisor-mode timer interrupt pending' },
               'VSSIP' : { 'high_bit':  2, 'low_bit':   2, 'readonly': False, 'desc': 'Virtual supervisor-mode software interrupt pending' },
             },
             'context': 'HART',
           },
    0x645: {
             'name': 'hvip',
             'desc': 'Hypervisor virtual-interrupt-pending register',
             'fields': {
               'WPRI'  : { 'high_bit': 63, 'low_bit':  11, 'readonly': True , 'desc': 'reserved' },
               'VSEIP' : { 'high_bit': 10, 'low_bit':  10, 'readonly': False, 'desc': 'Virtual supervisor-mode external interrupt pending' },
               'VSTIP' : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Virtual supervisor-mode timer interrupt pending' },
               'VSSIP' : { 'high_bit':  2, 'low_bit':   2, 'readonly': False, 'desc': 'Virtual supervisor-mode software interrupt pending' },
             },
             'context': 'HART',
           },
    0x64a: {
             'name': 'htinst',
             'desc': 'Hypervisor trap instruction register',
             'fields': {},
             'context': 'HART',
           },
    0xe12: {
             'name': 'hgeip',
             'desc': 'Hypervisor guest external interrupt pending',
             'fields': {},
             'context': 'HART',
           },

    # Hypervisor Configuration
    0x60a: {
             'name': 'henvcfg',
             'desc': 'Hypervisor environment configuration register',
             'fields': {
               'WPRI'  : { 'high_bit': 63, 'low_bit':   8, 'readonly': False, 'desc': 'reserved' },
               'CBZE'  : { 'high_bit':  7, 'low_bit':   7, 'readonly': False, 'desc': 'Cache block zero instruction enable' },
               'CBCFE' : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Cache block clean and flush instruction enable' },
               'CBIE'  : { 'high_bit':  5, 'low_bit':   4, 'readonly': False, 'desc': 'Cache block invalidate instruction enable' },
               'FIOM'  : { 'high_bit':  0, 'low_bit':   0, 'readonly': False, 'desc': 'Fence of I/O implies Memory' },
             },
             'context': 'HART',
           },

    # Hypervisor Protection and Translation
    0x680: {
             'name': 'hgatp',
             'desc': 'Hypervisor guest address translation and protection register',
             'fields': {
               'MODE' : { 'high_bit': 63, 'low_bit':  60, 'readonly': False, 'desc': 'Type of translation' },
               'WPRI' : { 'high_bit': 59, 'low_bit':  58, 'readonly': True , 'desc': 'reserved' },
               'VMID' : { 'high_bit': 57, 'low_bit':  44, 'readonly': False, 'desc': 'Virtual machine identifier' },
               'PPN'  : { 'high_bit': 43, 'low_bit':   0, 'readonly': False, 'desc': 'Physical page number' },
             },
             'context': 'HART',
           },

    # Hypervisor Debug/Trace Registers
    0x6a8: {
             'name': 'hcontext',
             'desc': 'Hypervisor mode context register',
             'fields': {},
             'context': 'HART',
           },

    # Hypervisor Counter/Timer Virtualization Registesr
    0x605: {
             'name': 'htimedelta',
             'desc': 'Hypervisor time delta register',
             'fields': {},
             'context': 'HART',
           },

    # Hypervisor State Enable Registers
    0x60c: {
             'name': 'hstateen0',
             'desc': 'Hypervisor State Enable Register 0',
             'fields': {
               'SE0'     : { 'high_bit': 63, 'low_bit':  63, 'readonly': False, 'desc': 'sstateen0 register access' },
               'ENVCFG'  : { 'high_bit': 62, 'low_bit':  62, 'readonly': False, 'desc': 'senvcfg register access' },
               'WPRI'    : { 'high_bit': 52, 'low_bit':   3, 'readonly': True , 'desc': 'Reserved' },
               'CSRIND'  : { 'high_bit': 60, 'low_bit':  60, 'readonly': False, 'desc': 'Sscsrind access' },
               'AIA'     : { 'high_bit': 59, 'low_bit':  59, 'readonly': False, 'desc': 'Ssaia state access' },
               'IMSIC'   : { 'high_bit': 58, 'low_bit':  58, 'readonly': False, 'desc': 'IMSIC state access' },
               'CONTEXT' : { 'high_bit': 57, 'low_bit':  57, 'readonly': False, 'desc': 'scontext register access' },
               'CTR'     : { 'high_bit': 54, 'low_bit':  54, 'readonly': False, 'desc': 'CTR register access' },
               'CLIC'    : { 'high_bit': 53, 'low_bit':  53, 'readonly': False, 'desc': 'CLIC registers access' },
               'JVT'     : { 'high_bit':  2, 'low_bit':   2, 'readonly': False, 'desc': 'JVT register access' },
               'FCSR'    : { 'high_bit':  1, 'low_bit':   1, 'readonly': False, 'desc': 'FCSR register access' },
               'C'       : { 'high_bit':  0, 'low_bit':   0, 'readonly': True , 'desc': 'Custom state access' },
             },
             'extension': 'Smstateen',
             'context': 'HART',
           },
    0x60d: {
             'name': 'hstateen1',
             'desc': 'Hypervisor State Enable Register 1',
             'fields': {},
             'extension': 'Smstateen',
             'context': 'HART',
           },
    0x60e: {
             'name': 'hstateen2',
             'desc': 'Hypervisor State Enable Register 2',
             'fields': {},
             'extension': 'Smstateen',
             'context': 'HART',
           },
    0x60f: {
             'name': 'hstateen3',
             'desc': 'Hypervisor State Enable Register 3',
             'fields': {},
             'extension': 'Smstateen',
             'context': 'HART',
           },

    # Virtual Supervisor Registers
    0x200: {
             'name': 'vsstatus',
             'desc': 'Virtual superviosr status register',
             'fields': {
               'SD'   : { 'high_bit': 63, 'low_bit':  63, 'readonly': False, 'desc': 'Fast-check bit: summarizes either FS/XS/VS set' },
               'WPRI' : { 'high_bit':  0, 'low_bit':   0, 'readonly': True , 'desc': 'resv' },
               'UXL'  : { 'high_bit': 33, 'low_bit':  32, 'readonly': True , 'desc': 'Controls XLEN value for U-mode' },
               'MXR'  : { 'high_bit': 19, 'low_bit':  19, 'readonly': False, 'desc': 'Make executable readable' },
               'SUM'  : { 'high_bit': 18, 'low_bit':  18, 'readonly': False, 'desc': 'Permit supervisor user memory access' },
               'XS'   : { 'high_bit': 16, 'low_bit':  15, 'readonly': False, 'desc': 'Extension status' },
               'FS'   : { 'high_bit': 14, 'low_bit':  13, 'readonly': False, 'desc': 'Floating-point status' },
               'VS'   : { 'high_bit': 10, 'low_bit':   9, 'readonly': False, 'desc': 'Vector status' },
               'SPP'  : { 'high_bit':  8, 'low_bit':   8, 'readonly': False, 'desc': 'Supervisor previous privilege mode' },
               'UBE'  : { 'high_bit':  6, 'low_bit':   6, 'readonly': False, 'desc': 'Controls endianness of explicit memory accesses made from U-mode' },
               'SPIE' : { 'high_bit':  5, 'low_bit':   5, 'readonly': False, 'desc': 'Indicates whether S-mode interrupts were enabled prior to trapping into S-mode' },
               'SIE'  : { 'high_bit':  1, 'low_bit':   1, 'readonly': False, 'desc': 'Enable/Disable interrupts in S-mode' },
             },
             'context': 'HART',
           },
    0x204: {
             'name': 'vsie',
             'desc': 'Virtual supervisor interrupt-enable register',
             'fields': {
               'WPRI' : { 'high_bit':  3, 'low_bit':   2, 'readonly': True , 'desc': 'undocumented' },
               'SEIE' : { 'high_bit':  9, 'low_bit':   9, 'readonly': False, 'desc': 'Enable supervisor-mode external interrupts' },
               'UEIE' : { 'high_bit':  8, 'low_bit':   8, 'readonly': True , 'desc': 'Enable user-mode external interrupts' },
               'STIE' : { 'high_bit':  5, 'low_bit':   5, 'readonly': False, 'desc': 'Enable supervisor-mode timer interrupts' },
               'UTIE' : { 'high_bit':  4, 'low_bit':   4, 'readonly': True , 'desc': 'Enable user-mode timer interrupts' },
               'SSIE' : { 'high_bit':  1, 'low_bit':   1, 'readonly': False, 'desc': 'Enable supervisor-mode software interrupts' },
               'USIE' : { 'high_bit':  0, 'low_bit':   0, 'readonly': True , 'desc': 'Enable user-mode software interrupts' },
             },
             'context': 'HART',
           },
    0x205: {
             'name': 'vstvec',
             'desc': 'Virtual supervisor trap vector base address register',
             'fields': {
               'BASE' : { 'high_bit': 63, 'low_bit':   2, 'readonly': False, 'desc': 'Vector base address' },
               'MODE' : { 'high_bit':  1, 'low_bit':   0, 'readonly': False, 'desc': '0 - Direct, 1 - Vectored' },
             },
             'context': 'HART',
           },
    0x240: {
             'name': 'vsscratch',
             'desc': 'Virtual supervisor scratch register',
             'fields': {},
             'context': 'HART',
           },
    0x241: {
             'name': 'vsepc',
             'desc': 'Virtual supervisor exception program counter',
             'fields': {},
             'context': 'HART',
           },
    0x242: {
             'name': 'vscause',
             'desc': 'Virtual supervisor cause register',
             'fields': {
               'Interrupt' : { 'high_bit': 63, 'low_bit':  63, 'readonly': False, 'desc': 'Bit to set a trap' },
               'Code'      : { 'high_bit': 62, 'low_bit':   0, 'readonly': False, 'desc': 'The exception code' },
             },
             'context': 'HART',
           },
    0x243: {
             'name': 'vstval',
             'desc': 'Virtual supervisor trap value register',
             'fields': {},
             'context': 'HART',
           },
    0x244: {
             'name': 'vsip',
             'desc': 'Virtual supervisor interrupt-pending register',
             'fields': {
               'WPRI' : { 'high_bit':  3, 'low_bit':   2, 'readonly': True , 'desc': 'undocumented' },
               'SEIP' : { 'high_bit':  9, 'low_bit':   9, 'readonly': False, 'desc': 'Supervisor-mode external interrupt pending' },
               'UEIP' : { 'high_bit':  8, 'low_bit':   8, 'readonly': True , 'desc': 'User-mode external interrupts pending' },
               'STIP' : { 'high_bit':  5, 'low_bit':   5, 'readonly': False, 'desc': 'Supervisor-mode timer interrupt pending' },
               'UTIP' : { 'high_bit':  4, 'low_bit':   4, 'readonly': True , 'desc': 'User-mode Machine timer interrupt pending' },
               'SSIP' : { 'high_bit':  1, 'low_bit':   1, 'readonly': False, 'desc': 'Supervisor-mode software interrupt pending' },
               'USIP' : { 'high_bit':  0, 'low_bit':   0, 'readonly': True , 'desc': 'User-mode software interrupt pending' },
             },
             'context': 'HART',
           },
    0x280: {
             'name': 'vsatp',
             'desc': 'Virtual interrupt-pending register',
             'fields': {
               'MODE' : { 'high_bit': 63, 'low_bit':  60, 'readonly': False, 'desc': 'Type of translation' },
               'ASID' : { 'high_bit': 59, 'low_bit':  44, 'readonly': False, 'desc': 'Address space identifier' },
               'PPN'  : { 'high_bit': 43, 'low_bit':   0, 'readonly': False, 'desc': 'Physical page number' },
             },
             'context': 'HART',
           },
}
