import json

# Command: "state.xlen"
# Return:  xlen (int)
def atlas_xlen(endpoint):
    return endpoint.request('state.xlen')

# Command: "state.pc"
# Return:  pc (int)
def atlas_pc(endpoint):
    return endpoint.request('state.pc')

# Command: "state.exit_code"
# Return:  exit code (int)
def atlas_exit_code(endpoint):
    return endpoint.request('state.exit_code')

# Command: "state.test_passed"
# Return:  success (bool)
def atlas_test_passed(endpoint):
    return endpoint.request('state.test_passed')

# Command "state.sim_stopped"
# Return:  stopped (bool)
def atlas_sim_stopped(endpoint):
    return endpoint.request('state.sim_stopped')

# Command: "state.inst_count"
# Return:  count (int)
def atlas_inst_count(endpoint):
    return endpoint.request('state.inst_count')

# Command: "state.sim_stopped"
# Return:  stopped (bool)
def atlas_sim_stopped(endpoint):
    return endpoint.request('state.sim_stopped')

# Command: "inst.uid"
# Return:  uid (int)
def atlas_inst_uid(endpoint):
    return endpoint.request('inst.uid')

# Command: "inst.mnemonic"
# Return:  mnemonic (str)
def atlas_inst_mnemonic(endpoint):
    return endpoint.request('inst.mnemonic')

# Command: "inst.dasm_string"
# Return:  dasm (str)
def atlas_inst_dasm_string(endpoint):
    return endpoint.request('inst.dasm_string')

# Command: "inst.opcode"
# Return:  opcode (int)
def atlas_inst_opcode(endpoint):
    return endpoint.request('inst.opcode')

# Command: "inst.priv"
# Return:  priv (int)
def atlas_inst_priv(endpoint):
    return endpoint.request('inst.priv')

# Command: "inst.has_immediate"
# Return:  flag (bool)
def atlas_inst_has_immediate(endpoint):
    return endpoint.request('inst.has_immediate')

# Command: "inst.immediate"
# Return:  imm value (int)
def atlas_inst_immediate(endpoint):
    return endpoint.request('inst.immediate')

# Command: "inst.rs1.name"
# Return:  rs1 name (str, or empty string if no rs1)
def atlas_inst_rs1_name(endpoint):
    rs1_name = endpoint.request('inst.rs1.name')
    if isinstance(rs1_name, str) and rs1_name != '':
        return rs1_name

    return None

# Command: "inst.rs2.name"
# Return:  rs2 name (str, or empty string if no rs2)
def atlas_inst_rs2_name(endpoint):
    rs2_name = endpoint.request('inst.rs2.name')
    if isinstance(rs2_name, str) and rs2_name != '':
        return rs2_name

    return None

# Command: "inst.rd.name"
# Return:  rd name (str, or empty string if no rd)
def atlas_inst_rd_name(endpoint):
    rd_name = endpoint.request('inst.rd.name')
    if isinstance(rd_name, str) and rd_name != '':
        return rd_name

    return None

# Command: "inst.active_exception"
# Return:  cause (int, or -1 for no exception)
def atlas_inst_active_exception(endpoint):
    return endpoint.request('inst.active_exception')

# Command: "state.num_regs_in_group"
# Return:  count (int)
def atlas_num_regs_in_group(endpoint, group_num):
    return endpoint.request('state.num_regs_in_group %d' % group_num)

# Command: "csr.name"
# Args:    csr number (int)
# Return:  csr name (str|error)
def atlas_csr_name(endpoint, csr_num):
    return endpoint.request('csr.name %d' % csr_num)

# Command: "reg.group_num"
# Args:    reg name (str)
# Return:  group number (int)
def atlas_reg_group_num(endpoint, reg_name):
    return endpoint.request('reg.group_num %s' % reg_name)

# Command: "reg.reg_id"
# Args:    reg name (str)
# Return:  register id (int)
def atlas_reg_id(endpoint, reg_name):
    return endpoint.request('reg.reg_id %s' % reg_name)

# Command: "reg.value"
# Args:    reg name (str)
# Return:  value (int)
def atlas_reg_value(endpoint, reg_name):
    return endpoint.request('reg.value %s' % reg_name)

# Command: "reg.write"
# Args:    reg name (str), value (int)
# Return:  ack
def atlas_reg_write(endpoint, reg_name, value):
    return endpoint.request('reg.write %s %d' % (reg_name, value))

# Command: "reg.dmiwrite"
# Args:    reg name (str), value (int)
# Return:  ack
def atlas_reg_dmiwrite(endpoint, reg_name, value):
    return endpoint.request('reg.dmiwrite %s %d' % (reg_name, value))

# Command: "break.action"
# Args:    action (str; pre_execute, pre_exception, or post_execute)
# Return:  ack
def atlas_break_action(endpoint, action):
    return endpoint.request('break.action %s' % action)

# Command: "state.finish_execute"
# Return:  action (str; pre_execute, pre_exception, post_execute, or sim_finished)
def atlas_finish_execute(endpoint):
    return endpoint.request('state.finish_execute')

# Command: "state.continue"
# Return:  action (str; pre_execute, pre_exception, post_execute, or sim_finished)
def atlas_continue(endpoint):
    return endpoint.request('state.continue')

# Command: "state.finish_sim"
# Return:  ack
def atlas_finish_sim(endpoint):
    return endpoint.request('state.finish_sim')

# Command: None (does not ping the simulator)
# Return:  alive (bool)
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

class AckResponse:
    def __init__(self, ack):
        self.ack = ack

class ErrorResponse:
    def __init__(self, err):
        self.err = err

class WarningResponse:
    def __init__(self, warn):
        self.warn = warn

class BrokenPipeResponse:
    def __init__(self):
        pass
