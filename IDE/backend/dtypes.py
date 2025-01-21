from editor.sim_api import *

# These classes act as stateless wrappers around various C++ objects
# in a running simulation.

class AtlasState:
    def __init__(self, endpoint):
        self.endpoint = endpoint

    def getXlen(self):
        return 64

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

    def getCSRs(self):
        csrs = {}
        for reg_id in range(atlas_num_csr_regs(self.endpoint)):
            reg_name = atlas_csr_name(self.endpoint, reg_id)
            if isinstance(reg_name, str):
                csr_val = atlas_reg_value(self.endpoint, reg_name)
                csrs[reg_name] = csr_val

        return csrs

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
        self._reg_name = reg_name

    @property
    def reg_name(self):
        return self._reg_name

    def getName(self):
        return self._reg_name

    def getGroupNum(self):
        return atlas_reg_group_num(self.endpoint, self._reg_name)

    def getID(self):
        return atlas_reg_id(self.endpoint, self._reg_name)

    def read(self):
        return atlas_reg_value(self.endpoint, self._reg_name)

    def write(self, value):
        atlas_reg_write(self.endpoint, self._reg_name, value)

    def dmiWrite(self, value):
        atlas_reg_dmiwrite(self.endpoint, self._reg_name, value)

    def deepCopy(self):
        return SpartaRegisterDeepCopy(self)

class DynamicSpartaRegister(SpartaRegister):
    def __init__(self, endpoint, reg_name):
        super().__init__(endpoint, reg_name)

    def ReBindRegister(self, reg_name):
        self._reg_name = reg_name

class AtlasRS1(DynamicSpartaRegister): pass
class AtlasRS2(DynamicSpartaRegister): pass
class AtlasRD(DynamicSpartaRegister): pass

class SpartaRegisterDeepCopy:
    def __init__(self, orig_reg):
        self._reg_name = orig_reg.getName()
        self._group_num = orig_reg.getGroupNum()
        self._reg_id = orig_reg.getID()
        self._value = orig_reg.read()

    def getName(self):
        return self._reg_name

    def getGroupNum(self):
        return self._group_num

    def getID(self):
        return self._reg_id

    def read(self):
        return '0x' + format(self._value, '08x')

### ====================================================================
### Direct sim object instantiation
def atlas_state(endpoint):
    return AtlasState(endpoint)

def atlas_current_inst(endpoint):
    inst = AtlasInst(endpoint)
    return inst if isinstance(inst.getUid(), int) else None
