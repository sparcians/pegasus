import json

class SimResponse:
    def __init__(self, response=None):
        self.response = response

class AckResponse(SimResponse): pass
class ErrorResponse(SimResponse): pass
class WarningResponse(SimResponse): pass
class BrokenPipeResponse(SimResponse): pass

# Equiv C++:  AtlasState::getXlen()
def atlas_xlen(endpoint):
    return endpoint.request('state.xlen')

# Equiv C++:  AtlasState::getPc()
def atlas_pc(endpoint):
    return endpoint.request('state.pc')

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
    return endpoint.request('inst.opcode')

# Equiv C++:  AtlasState::getCurrentInst()->getPrivMode()
def atlas_inst_priv(endpoint):
    return endpoint.request('inst.priv')

# Equiv C++:  AtlasState::getCurrentInst()->hasImmediate()
def atlas_inst_has_immediate(endpoint):
    return endpoint.request('inst.has_immediate')

# Equiv C++:  AtlasState::getCurrentInst()->getImmediate()
def atlas_inst_immediate(endpoint):
    return endpoint.request('inst.immediate')

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
    return endpoint.request('reg.value %s' % reg_name)

# Equiv C++:  AtlasState::findRegister(reg_name)->write(value)
def atlas_reg_write(endpoint, reg_name, value):
    return endpoint.request('reg.write %s %d' % (reg_name, value))

# Equiv C++:  AtlasState::findRegister(reg_name)->dmiWrite<uint64_t>(value)
def atlas_reg_dmiwrite(endpoint, reg_name, value):
    return endpoint.request('reg.dmiwrite %s %d' % (reg_name, value))

# Set a breakpoint at the given execute phase (pre_execute, pre_exception, post_execute).
def atlas_break_action(endpoint, action):
    return endpoint.request('sim.break %s' % action)

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
    return endpoint.request('sim.finish_execute')

# Continue the running simulation until the next breakpoint is hit
# or the simulation finishes.
#
# Returns the simulation/execution phase that the simulator is in
# which is one of pre_execute, pre_exception, post_execute, or
# sim_finished.
def atlas_continue(endpoint):
    return endpoint.request('sim.continue')

# Finish the simulation now. The simulation will complete and the
# simulator will exit. This invalidates further use of the endpoint.
def atlas_finish_sim(endpoint):
    return endpoint.request('sim.finish')

# Ping the Atlas C++ simulator to see if it is still alive.
def atlas_sim_alive(endpoint):
    return not isinstance(atlas_pc(endpoint), BrokenPipeResponse)

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
