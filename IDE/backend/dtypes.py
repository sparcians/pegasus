from backend.sim_api import *

# These classes act as stateless wrappers around various C++ objects
# in a running simulation.

class AtlasState:
    def __init__(self, endpoint):
        self.endpoint = endpoint

    def getXlen(self):
        return atlas_xlen(self.endpoint)

    def getPc(self):
        return atlas_pc(self.endpoint)

    def getPrivMode(self):
        return atlas_inst_priv(self.endpoint)

    def getTrapCause(self):
        return atlas_inst_active_exception(self.endpoint)

    def getSimState(self):
        return AtlasState.SimState(self.endpoint)

    def getCurrentInst(self):
        return AtlasState.AtlasInst(self.endpoint)

    def getIntRegisterSet(self):
        return AtlasIntRegisterSet(self.endpoint)

    def getFpRegisterSet(self):
        return AtlasFpRegisterSet(self.endpoint)
    
    def getVectorRegisterSet(self):
        return AtlasVecRegisterSet(self.endpoint)
    
    def getCsrRegisterSet(self):
        return AtlasCsrRegisterSet(self.endpoint)

    class SimState:
        def __init__(self, endpoint):
            self.endpoint = endpoint

        @property
        def inst_count(self):
            return atlas_inst_count(self.endpoint)

        @property
        def sim_stopped(self):
            return atlas_sim_stopped(self.endpoint)

        @property
        def test_passed(self):
            return atlas_test_passed(self.endpoint)

        @property
        def workload_exit_code(self):
            return atlas_exit_code(self.endpoint)

class AtlasInst:
    def __init__(self, endpoint):
        self.endpoint = endpoint

    def getAtlasState(self):
        return AtlasState(self.endpoint)

    def getUid(self):
        return atlas_inst_uid(self.endpoint)

    def getMnemonic(self):
        return atlas_inst_mnemonic(self.endpoint)

    def dasmString(self):
        return atlas_inst_dasm_string(self.endpoint)

    def getOpcode(self):
        return atlas_inst_opcode(self.endpoint)

    def getPrivMode(self):
        return atlas_inst_priv(self.endpoint)

    def getRs1(self):
        rs1_name = atlas_inst_rs1_name(self.endpoint)
        return AtlasRS1(self.endpoint, rs1_name) if rs1_name else None

    def getRs2(self):
        rs2_name = atlas_inst_rs2_name(self.endpoint)
        return AtlasRS2(self.endpoint, rs2_name) if rs2_name else None

    def getRd(self):
        rd_name = atlas_inst_rd_name(self.endpoint)
        return AtlasRD(self.endpoint, rd_name) if rd_name else None

    def getImmediate(self):
        has_imm = atlas_inst_has_immediate(self.endpoint)
        return atlas_inst_immediate(self.endpoint) if has_imm else None

    def deepCopy(self):
        return AtlasInstDeepCopy(self)

class AtlasInstDeepCopy:
    def __init__(self, orig_inst):
        self.uid = orig_inst.getUid()
        self.mnemonic = orig_inst.getMnemonic()
        self.dasm = orig_inst.dasmString()
        self.opcode = orig_inst.getOpcode()
        self.priv = orig_inst.getPrivMode()

        rs1 = orig_inst.getRs1()
        rs2 = orig_inst.getRs2()
        rd = orig_inst.getRd()
        imm = orig_inst.getImmediate()

        self.rs1 = rs1.deepCopy() if rs1 else None
        self.rs2 = rs2.deepCopy() if rs2 else None
        self.rd = rd.deepCopy() if rd else None
        self.imm = imm

    def getAtlasState(self):
        return None

    def getUid(self):
        return self.uid

    def getMnemonic(self):
        return self.mnemonic
    
    def dasmString(self):
        return self.dasm
    
    def getOpcode(self):
        return self.opcode
    
    def getPrivMode(self):
        return self.priv
    
    def getRs1(self):
        return self.rs1
    
    def getRs2(self):
        return self.rs2
    
    def getRd(self):
        return self.rd
    
    def getImmediate(self):
        return self.imm

class SpartaRegister:
    def __init__(self, endpoint, reg_name):
        self.endpoint = endpoint
        self.reg_name = reg_name

    def getName(self):
        return self.reg_name

    def getGroupNum(self):
        return atlas_reg_group_num(self.endpoint, self.reg_name)

    def getID(self):
        return atlas_reg_id(self.endpoint, self.reg_name)

    def read(self):
        return atlas_reg_value(self.endpoint, self.reg_name)

    def write(self, value):
        atlas_reg_write(self.endpoint, self.reg_name, value)

    def dmiWrite(self, value):
        atlas_reg_dmiwrite(self.endpoint, self.reg_name, value)

    def deepCopy(self):
        return SpartaRegisterDeepCopy(self)

class AtlasRD(SpartaRegister):  pass
class AtlasRS1(SpartaRegister): pass
class AtlasRS2(SpartaRegister): pass

# Provide read-only register access after the simulation is
# over. This may be useful to some python observers for post-
# processing, and the simulation would be stopped at this point.
#
# Create from reg.deepCopy()
class SpartaRegisterDeepCopy:
    def __init__(self, orig_reg):
        self.reg_name = orig_reg.getName()
        self.group_num = orig_reg.getGroupNum()
        self.reg_id = orig_reg.getID()
        self.value = orig_reg.read()

    def getName(self):
        return self.reg_name

    def getGroupNum(self):
        return self.group_num

    def getID(self):
        return self.reg_id

    def read(self):
        return '0x{:08x}'.format(self.value)

    def deepCopy(self):
        return SpartaRegisterDeepCopy(self)

class AtlasRegisterSet:
    def __init__(self, endpoint, group_num):
        self.endpoint = endpoint
        self.group_num = group_num

    def getRegister(self, reg_id):
        if reg_id >= self.getNumRegisters():
            return None

        prefix = ['x', 'f', 'v'][self.group_num]
        reg_name = prefix + str(reg_id)
        return SpartaRegister(self.endpoint, reg_name)

    def getNumRegisters(self):
        return atlas_num_regs_in_group(self.endpoint, self.group_num)

class AtlasIntRegisterSet(AtlasRegisterSet): pass
class AtlasFpRegisterSet(AtlasRegisterSet):  pass
class AtlasVecRegisterSet(AtlasRegisterSet): pass

class AtlasCsrRegisterSet(AtlasRegisterSet):
    def __init__(self, endpoint):
        AtlasRegisterSet.__init__(self, endpoint, 3)

    def getRegister(self, reg_id):
        csr_name = atlas_csr_name(self.endpoint, reg_id)
        if isinstance(csr_name, str) and csr_name != '':
            return SpartaRegister(self.endpoint, csr_name)

        return None

TRAP_CAUSES = [
    'MISSALIGNED_FETCH',
    'FETCH_ACCESS',
    'ILLEGAL_INSTRUCTION',
    'BREAKPOINT',
    'MISSALIGNED_LOAD',
    'LOAD_ACCESS',
    'MISSALIGNED_STORE',
    'STORE_ACCESS',
    'USER_ECALL',
    'SUPERVISOR_ECALL',
    'VIRTUAL_SUPERVISOR_ECALL',
    'MACHINE_ECALL',
    'FETCH_PAGE_FAULT',
    'LOAD_PAGE_FAULT',
    'STORE_PAGE_FAULT',
    'DOUBLE_TRAP',
    'SOFTWARE_CHECK_FAULT',
    'HARDWARE_ERROR_FAULT',
    'FETCH_GUEST_PAGE_FAULT',
    'LOAD_GUEST_PAGE_FAULT',
    'VIRTUAL_INSTRUCTION',
    'STORE_GUEST_PAGE_FAULT'
]

# Create a wrapper around a running AtlasState C++ object.
def atlas_state(endpoint):
    return AtlasState(endpoint)

# Create a wrapper around a running AtlasInst C++ object.
# C++: "AtlasInstPtr AtlasState::getCurrentInst()"
def atlas_current_inst(endpoint):
    inst = AtlasInst(endpoint)
    return inst if isinstance(inst.getUid(), int) else None
