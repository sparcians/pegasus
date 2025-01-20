import json

# JSON conversion to help with the Atlas IDE (IPC using JSON messages)
class JsonConverter:
    @classmethod
    def ConvertResponse(cls, response):
        obj = json.loads(response)
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
        
        if response_type == 'inst':
            return AtlasInst(**response_payload)

        if response_type == 'state':
            return AtlasState(**response_payload)

        if response_type == 'simstate':
            return SimState(**response_payload)

        if response_type == 'jsonfile':
            with open(response_payload, 'r') as fin:
                return json.load(fin)

        if response_type == 'regdumpfile':
            with open(response_payload, 'r') as fin:
                regdump_args = json.load(fin)
                return RegisterDump(**regdump_args)

        if response_type == 'regdump':
            regdump_args = json.loads(response_payload)
            return RegisterDump(**regdump_args)

        if response_type == 'breakpoints':
            bp_args = json.loads(response_payload)
            return ActiveBreakpoints(**bp_args)

class AckResponse:
    def __init__(self, ack):
        self.ack = ack

class ErrorResponse:
    def __init__(self, err):
        self.err = err

class WarningResponse:
    def __init__(self, warn):
        self.warn = warn

class AtlasInst:
    def __init__(self, **kwargs):
        self.mnemonic = kwargs['mnemonic']
        self.dasm = kwargs['dasm_string']
        self.opcode = kwargs['opcode']
        self.opcode_size = kwargs['opcode_size']
        self.priv = kwargs['priv']
        self.is_memory_inst = kwargs['is_memory_inst']
        self.immediate = kwargs.get('immediate', None)
        self.rs1 = kwargs.get('rs1', None)
        self.rs1val = kwargs.get('rs1val', None)
        self.rs2 = kwargs.get('rs2', None)
        self.rs2val = kwargs.get('rs2val', None)
        self.rd = kwargs.get('rd', None)
        self.rdval = kwargs.get('rdval', None)

class AtlasState:
    def __init__(self, **kwargs):
        pass

class SimState:
    def __init__(self, **kwargs):
        self.workload_exit_code = kwargs['workload_exit_code']
        self.test_passed = kwargs['test_passed']
        self.sim_stopped = kwargs['sim_stopped']
        self.inst_count = kwargs['inst_count']

class RegisterDump:
    def __init__(self, **kwargs):
        pass

class ActiveBreakpoints:
    def __init__(self, **kwargs):
        pass
