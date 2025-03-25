import json
import backend.c_dtypes as c_dtypes

def FormatHex(val):
    if val is None:
        return 'NULL'

    if isinstance(val, int):
        return FormatHex(hex(val))

    assert isinstance(val, str)

    if val.startswith('-0x'):
        val = val.lstrip('-0x')
        val = val.lstrip('0')
        if val == '':
            return '-0x0'
        else:
            return '-0x' + val

    if val.startswith('0x'):
        val = val.lstrip('0x')
        val = val.lstrip('0')
        if val == '':
            return '0x0'
        else:
            return '0x' + val

    if val == 'NULL':
        return 'NULL'

    return hex(int(val))

class SimResponse:
    def __init__(self, response=None):
        self.response = response

class AckResponse(SimResponse):
    def __bool__(self):
        return True

class ErrorResponse(SimResponse):
    def __bool__(self):
        return False

class WarningResponse(SimResponse):
    def __bool__(self):
        return False

class BrokenPipeResponse(SimResponse):
    def __bool__(self):
        return False

# Equiv C++:  AtlasState::getXlen()
def atlas_xlen(endpoint):
    return endpoint.request('state.xlen')

# Equiv C++:  AtlasState::getPc()
def atlas_pc(endpoint):
    pc = endpoint.request('state.pc')
    if type(pc) in (int, str):
        pc = FormatHex(pc)

    return pc

# Equiv C++:  AtlasState::getPrevPc()
def atlas_prev_pc(endpoint):
    pc = endpoint.request('state.prev_pc')
    if type(pc) in (int, str):
        pc = FormatHex(pc)

    return pc

# Equiv C++:  AtlasState::getSimState()->current_uid
def atlas_current_uid(endpoint):
    return endpoint.request('state.current_uid')

# Equiv C++:  AtlasState::getSimState()->workload_exit_code
def atlas_exit_code(endpoint):
    return endpoint.request('state.exit_code')

# Equiv C++:  AtlasState::getSimState()->test_passed
def atlas_test_passed(endpoint):
    return endpoint.request('state.test_passed')

# Equiv C++:  AtlasState::getSimState()->sim_stopped
def atlas_sim_stopped(endpoint):
    return endpoint.request('state.sim_stopped')

# Equiv C++:  AtlasState::getSimState()->inst_count
def atlas_inst_count(endpoint):
    return endpoint.request('state.inst_count')

# Equiv C++:  AtlasState::getCurrentInst()->getUid()
def atlas_inst_uid(endpoint):
    return endpoint.request('inst.uid')

# Equiv C++:  AtlasState::getCurrentInst()->getMnemonic()
def atlas_inst_mnemonic(endpoint):
    return endpoint.request('inst.mnemonic')

# Equiv C++:  AtlasState::getCurrentInst()->dasmString()
def atlas_inst_dasm_string(endpoint):
    return endpoint.request('inst.dasm_string')

# Equiv C++:  AtlasState::getCurrentInst()->getOpcode()
def atlas_inst_opcode(endpoint):
    opcode = endpoint.request('inst.opcode')
    if type(opcode) in (int, str):
        opcode = FormatHex(opcode)

    return opcode

# Equiv C++:  AtlasState::getCurrentInst()->getPrivMode()
def atlas_inst_priv(endpoint):
    return endpoint.request('inst.priv')

# Equiv C++:  AtlasState::getCurrentInst()->hasImmediate()
def atlas_inst_has_immediate(endpoint):
    return endpoint.request('inst.has_immediate')

# Equiv C++:  AtlasState::getCurrentInst()->getImmediate()
def atlas_inst_immediate(endpoint):
    imm = endpoint.request('inst.immediate')
    if type(imm) in (int, str):
        imm = FormatHex(imm)

    return imm

# Equiv C++:  AtlasState::getCurrentInst()->getRs1()->getName()
def atlas_inst_rs1_name(endpoint):
    rs1_name = endpoint.request('inst.rs1.name')
    if isinstance(rs1_name, str) and rs1_name != '':
        return rs1_name

    return None

# Equiv C++:  AtlasState::getCurrentInst()->getRs2()->getName()
def atlas_inst_rs2_name(endpoint):
    rs2_name = endpoint.request('inst.rs2.name')
    if isinstance(rs2_name, str) and rs2_name != '':
        return rs2_name

    return None

# Equiv C++:  AtlasState::getCurrentInst()->getRd()->getName()
def atlas_inst_rd_name(endpoint):
    rd_name = endpoint.request('inst.rd.name')
    if isinstance(rd_name, str) and rd_name != '':
        return rd_name

    return None

# Equiv C++:  AtlasState::getCurrentInst()->getExceptionUnit()->getUnhandledException()
def atlas_inst_active_exception(endpoint):
    return endpoint.request('inst.active_exception')

# Equiv C++:  AtlasState::get[Int|Fp|Vec|Csr]RegisterSet()->getNumRegisters()
def atlas_num_regs_in_group(endpoint, group_num):
    return endpoint.request('state.num_regs_in_group %d' % group_num)

# Equiv C++:  AtlasState::getCsrRegisterSet()->getRegister(csr_num)->getName()
def atlas_csr_name(endpoint, csr_num):
    return endpoint.request('csr.name %d' % csr_num)

# Equiv C++:  AtlasState::findRegister(reg_name)->getGroupNum()
def atlas_reg_group_num(endpoint, reg_name):
    return endpoint.request('reg.group_num %s' % reg_name)

# Equiv C++:  AtlasState::findRegister(reg_name)->getID()
def atlas_reg_id(endpoint, reg_name):
    return endpoint.request('reg.reg_id %s' % reg_name)

# Equiv C++:  AtlasState::findRegister(reg_name)->read()
def atlas_reg_value(endpoint, reg_name):
    reg_value = endpoint.request('reg.value %s' % reg_name)
    if type(reg_value) in (int, str):
        reg_value = FormatHex(reg_value)

    return reg_value

# Equiv C++:  AtlasState::findRegister(reg_name)->write(value)
def atlas_reg_write(endpoint, reg_name, value):
    value = c_dtypes.convert_to_hex(value)
    return endpoint.request('reg.write %s %s' % (reg_name, value))

# Equiv C++:  AtlasState::findRegister(reg_name)->dmiWrite<uint64_t>(value)
def atlas_reg_dmiwrite(endpoint, reg_name, value):
    value = c_dtypes.convert_to_hex(value)
    return endpoint.request('reg.dmiwrite %s %s' % (reg_name, value))

# Equiv C++:  AtlasState->getCurrentInst()
#                       ->getMavisOpcodeInfo()
#                       ->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
def atlas_inst_csr(endpoint):
    return endpoint.request('inst.csr.name')

# Equiv C++:  AtlasState->getCurrentInst()
#                       ->getMavisOpcodeInfo()
#                       ->getInstType();
def atlas_inst_type(endpoint):
    return endpoint.request('inst.type')


# Stop Atlas from executing C++ instruction handler code and jump right
# to the finish ActionGroup (increment PC, post_execute). This is useful
# for when Python code is to be injected into the simulation in place of
# the C++ code.
#
# For example, you could write an observer to tryout a new instruction
# implementation or to debug your simulator in Python for rapid prototyping:
#
#     class PythonInstRewriter(Observer):
#         def OnPreExecute(self, endpoint):
#             insn = atlas_current_inst(endpoint)
#             insn.getRd().write(insn.getRs1().read() * insn.getRs2().read())
#
#             # Tell Atlas to skip the C++ inst implementation.
#             atlas_finish_execute(endpoint)
#
def atlas_finish_execute(endpoint):
    return endpoint.request('sim.finish_execute', 'sim_dead')

# Set a breakpoint at the given execute phase.
# Must be one of pre_execute, pre_exception, or post_execute.
def atlas_break_action(endpoint, action):
    return endpoint.request('sim.break %s' % action)

# Continue the running simulation until the next breakpoint is hit
# or the simulation finishes.
#
# Returns the simulation/execution phase that the simulator is in
# which is one of pre_execute, pre_exception, post_execute, or
# sim_finished.
def atlas_continue(endpoint):
    return endpoint.request('sim.continue', 'sim_dead')

# Finish the simulation now. The simulation will complete and the
# simulator will exit. This invalidates further use of the endpoint.
def atlas_finish_sim(endpoint):
    return endpoint.request('sim.finish', 'sim_dead')

# Terminate a simulation early for any reason with the given exit code.
# This invalidates further use of the endpoint. The simulator's SimState
# will reflect the following:
#
#   sim_state.workload_exit_code = exit_code
#   sim_state.test_passed = false
#   sim_state.sim_stopped = true
def atlas_kill_sim(endpoint, exit_code):
    return endpoint.request('sim.kill %d' % exit_code)

### ====================================================================
### JSON conversion (IPC using JSON messages)
class JsonConverter:
    @classmethod
    def ConvertResponse(cls, response):
        obj = json.loads(response.replace('\t', '  '))
        response_code = obj['response_code']

        if response_code == 'err':
            return cls._ConvertError(obj)
        elif response_code == 'warn':
            return cls._ConvertWarning(obj)
        elif response_code == 'ok':
            return cls._ConvertResponse(obj)

        return None

    @classmethod
    def _ConvertError(cls, obj):
        assert 'response_type' not in obj or obj['response_type'] == 'str'
        errmsg = obj['response_payload']
        return ErrorResponse(errmsg)

    @classmethod
    def _ConvertWarning(cls, obj):
        assert 'response_type' not in obj or obj['response_type'] == 'str'
        errmsg = obj['response_payload']
        return WarningResponse(errmsg)

    @classmethod
    def _ConvertResponse(cls, obj):
        if 'response_payload' not in obj:
            return AckResponse(None)

        response_payload = obj['response_payload']
        if 'response_type' not in obj:
            return AckResponse(response_payload)

        response_type = obj['response_type']

        if response_type == 'int':
            return int(response_payload)

        if response_type == 'float':
            return float(response_payload)

        if response_type == 'str':
            return response_payload

        if response_type == 'bool':
            return response_payload
